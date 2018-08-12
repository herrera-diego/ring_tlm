/*******************************************************
 *  Instituto Tecnologico de Costa Rica
 *
 *  Diseño de Alto Nivel de Sistema Electrónicos
 *
 *  Proyecto 2 (Grupo 1)
 *
 *  Mario Castro    200827325
 *  Diego Herrera   200324558
 *  Fernando París  200510153
 *  Esteban Rivera  2018319491
 *  Kevin Víquez    200944341
 *******************************************************/

#include <queue>

#include "tlm_utils/peq_with_cb_and_phase.h"

#include "common_rand.h"
#include "constants.h"
#include "memory.h"


Memory::Memory(sc_core::sc_module_name module_name)
    : target_socket("target_socket"),
      m_payload_event_queue("Memory_PEQ", this, &Memory::peq_cb),
      n_trans(0),
      response_in_progress(false),
      next_response_pending(NULL),
      end_req_pending()
{
    init_memory();

    target_socket.register_nb_transport_fw(this, &Memory::nb_transport_fw);
}

void Memory::init_memory()
{
    // Initialize memory with random data   
    for (int i = 0; i < MEMORY_SIZE; i++)
    {  
        mem[i] = rand();
    }
}

tlm::tlm_sync_enum Memory::nb_transport_fw(tlm::tlm_generic_payload& trans,
                                           tlm::tlm_phase& phase, sc_time& delay)
{
    sc_dt::uint64    adr = trans.get_address();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();

    std::cout << ">>>>>>>>>> Outgoing msg received in Memory: " << name()
              << ", Transaction: " << decode_transID(trans.get_address())
              << ", Phase: " << phase
              << ", Addr: " << dec << decode_addr(adr)
              << ", Msg len: " << len
              << ", Data: " << *reinterpret_cast<int *>(&byt)
              << ", SRC: " << decode_src(trans.get_address())
              << ", DST: " << decode_dest(trans.get_address())
              << ", Delay: " << delay
              << ", Time: " << sc_time_stamp() << "\n";

    // Obliged to check the transaction attributes for unsupported features
    // and to generate the appropriate error response
    if (byt != 0) {

        trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
        return tlm::TLM_COMPLETED;
    }

    if (len > 4 || wid < len) {

        trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
        return tlm::TLM_COMPLETED;
    }

    // Now queue the transaction until the annotated time has elapsed
    m_payload_event_queue.notify(trans, phase, delay);
    return tlm::TLM_ACCEPTED;
}

void Memory::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
{
    tlm::tlm_sync_enum status;
    sc_time delay;

    std::cout << "<-<-<-<-<- Outgoing msg from Memory: " << name()
              << ", Transaction: " << decode_transID(trans.get_address())
              << ", Phase: " << phase
              << ", Cmd: " << (trans.get_command() ? 'W' : 'R')
              << ", Addr: " << dec << decode_addr(trans.get_address())
              << ", SRC: " << decode_src(trans.get_address())
              << ", DST: " << decode_dest(trans.get_address())
              << ", Time: " << sc_time_stamp() << "\n";

    switch (phase) {

    case tlm::BEGIN_REQ:
        // Increment the transaction reference count
        trans.acquire();

        // Put back-pressure on initiator by deferring END_REQ until pipeline is clear
        // (initiator is blocked until pending END_REQ comes out of queue)
        if (n_trans == 2) {

            end_req_pending.push(&trans);
        }
        else {

            status = send_end_req(trans);
            if (status == tlm::TLM_COMPLETED) // It is questionable whether this is valid
                break;
        }

        break;

    case tlm::END_RESP:
        // On receiving END_RESP, the target can release the transaction
        // and allow other pending transactions to proceed

        //if (!response_in_progress) {
        //    SC_REPORT_FATAL("TLM-2", "Illegal transaction phase END_RESP received by target");
        //}

        trans.release();
        n_trans--;

        // Target itself is now clear to issue the next BEGIN_RESP
        response_in_progress = false;

        if (next_response_pending) {

            send_response(*next_response_pending);
            next_response_pending = 0;
        }

        // ... and to unblock the initiator by issuing END_REQ
        if (!end_req_pending.empty()) {

            status = send_end_req(*end_req_pending.front());
            end_req_pending.pop();
        }

        break;

    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
            SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by target");
            break;

    default:
        if (phase == internal_ph) {

            // Execute the read or write commands
            tlm::tlm_command cmd = trans.get_command();
            sc_dt::uint64    adr = trans.get_address();
            unsigned char*   ptr = trans.get_data_ptr();
            unsigned int     len = trans.get_data_length();

            adr = decode_addr(adr);

            if (adr >= MEMORY_SIZE) {
                SC_REPORT_FATAL("TLM-2", "Attempt to execute command beyond memory boundaries!");
            }

            if ( cmd == tlm::TLM_READ_COMMAND ) {

                trans.set_data_ptr(reinterpret_cast<unsigned char *>(&mem[adr]));

                std::cout << "********** Processing msg in Memory: " << name()
                          << ", Transaction: " << decode_transID(trans.get_address())
                          << ", Executing: READ"
                          << ", Addr: " << dec << adr
                          << ", Data: " << mem[adr]
                          << ", SRC: " << decode_src(trans.get_address())
                          << ", DST: " << decode_dest(trans.get_address())
                          << ", Time: " << sc_time_stamp() << "\n";
            }
            else if ( cmd == tlm::TLM_WRITE_COMMAND ) {

                mem[adr] = *reinterpret_cast<int*>(ptr);

                std::cout << "********** Processing msg in Memory: " << name()
                          << ", Transaction: " << decode_transID(trans.get_address())
                          << ", Executing: WRITE"
                          << ", Addr: " << dec << adr
                          << ", Data: " << *reinterpret_cast<int*>(ptr)
                          << ", SRC: " << decode_src(trans.get_address())
                          << ", DST: " << decode_dest(trans.get_address())
                          << ", Time: " << sc_time_stamp() << "\n";
            }

            trans.set_response_status( tlm::TLM_OK_RESPONSE );

            // Target must honor BEGIN_RESP/END_RESP exclusion rule
            // i.e. must not send BEGIN_RESP until receiving previous END_RESP or BEGIN_REQ
            send_response(trans);
            /*
            if (response_in_progress) {
                std::cout << "RESPONSE IN PROGRESS!\n";
                // Target allows only two transactions in-flight
                if (next_response_pending)
                    SC_REPORT_FATAL("TLM-2", "Attempt to have two pending responses in target");
                next_response_pending = &trans;
            }
            else {
                std::cout << "RESPONSE NOT IN PROGRESS!\n";
                send_response(trans);
            }
            */

            break;
        }
    }
}

tlm::tlm_sync_enum Memory::send_end_req(tlm::tlm_generic_payload& trans)
{
    tlm::tlm_sync_enum status;
    tlm::tlm_phase bw_phase;
    tlm::tlm_phase int_phase = internal_ph;
    sc_time delay;

    // Queue the acceptance and the response with the appropriate latency
    bw_phase = tlm::END_REQ;
    delay = sc_time(rand_ps(), SC_PS); // Accept delay

    trans.set_address(compose_address(decode_transID(trans.get_address()),
                                      TOP_ROUTER,
                                      decode_src(trans.get_address()),
                                      decode_addr(trans.get_address())));
#if 0
    std::cout << "SEND_END_PEQ: " << name()
              << ", Transaction: " << decode_transID(trans.get_address())
              << ", Phase: " << bw_phase
              << ", SRC: " << decode_src(trans.get_address())
              << ", DST: " << decode_dest(trans.get_address())
              << ", Time: " << sc_time_stamp() << "\n";
#endif
    // Send Message to Router
    status = target_socket->nb_transport_bw( trans, bw_phase, delay );

    if (status == tlm::TLM_COMPLETED) {

        // Transaction aborted by the initiator
        // (TLM_UPDATED cannot occur at this point in the base protocol, so need not be checked)
        trans.release();
        return status;
    }

    // Queue internal event to mark beginning of response
    delay = delay + sc_time(rand_ps(), SC_PS); // Latency
    m_payload_event_queue.notify( trans, int_phase, delay );
    n_trans++;

    return status;
}

void Memory::send_response(tlm::tlm_generic_payload& trans)
{
    tlm::tlm_sync_enum status;
    tlm::tlm_phase bw_phase;
    sc_time delay;

    response_in_progress = true;
    bw_phase = tlm::BEGIN_RESP;
    delay = SC_ZERO_TIME;

#if 0
    std::cout << "SEND_RESPONSE: " << name()
              << ", Transaction: " << decode_transID(trans.get_address())
              << ", Phase: " << bw_phase
              << ", SRC: " << decode_src(trans.get_address())
              << ", DST: " << decode_dest(trans.get_address())
              << ", Time: " << sc_time_stamp() << "\n";
#endif

    // Send Message to Router
    status = target_socket->nb_transport_bw(trans, bw_phase, delay);

    if (status == tlm::TLM_UPDATED) {

        // The timing annotation must be honored
        m_payload_event_queue.notify(trans, bw_phase, delay);
    }
    else if (status == tlm::TLM_COMPLETED) {

        // The initiator has terminated the transaction
        trans.release();
        n_trans--;
        response_in_progress = false;
    }
}
