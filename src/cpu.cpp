#include "cpu.h"
#include "common_rand.h"
#include "constants.h"

CPU::CPU(sc_core::sc_module_name module_name, unsigned int id)
    : sc_module(module_name),
      init_socket("init_socket"),
      m_payload_event_queue(this, &CPU::peq_cb),
      cpu_id(id),
      request_in_progress(NULL)
{
    init_socket.register_nb_transport_bw(this, &CPU::nb_transport_bw);

    SC_THREAD(thread_process);
}

void CPU::thread_process()
{
    tlm::tlm_generic_payload* trans;
    tlm::tlm_phase phase;
    sc_time delay;

    // Generate a sequence of random transactions
    for (int i = 0; i < 2; i++)
    {
        int adr = rand() % MEMORY_SIZE;
        tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);
        
        if (cmd == tlm::TLM_WRITE_COMMAND) {
            //data[i % 16] = rand();
            data = rand();
        }

        // Grab a new transaction from the memory manager
        trans = m_mm.allocate();
        trans->acquire();

        // Set all attributes except byte_enable_length and extensions (unused)
        trans->set_command( cmd );
        trans->set_address( adr );
        //trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data[i % 16]) );
        trans->set_data_ptr( reinterpret_cast<unsigned char *>(&data) );
        trans->set_data_length( 4 );
        trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
        trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
        trans->set_dmi_allowed( false ); // Mandatory initial value
        trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

        // Initiator must honor BEGIN_REQ/END_REQ exclusion rule
        if (request_in_progress) {
            wait(end_request_event);
        }

        request_in_progress = trans;
        phase = tlm::BEGIN_REQ;

        // Timing annotation models processing time of initiator prior to call
        delay = sc_time(rand_ps(), SC_PS);
        
        std::cout << "Message from CPU: " << cpu_id << " Socket: " << init_socket.name() << " "
                  << hex << adr << " " << name() << " new, cmd=" << (cmd ? 'W' : 'R') << dec << ", data="
                  << hex << data << " at time " << sc_time_stamp() << endl;
        
        /*
        std::cout << hex << adr << " " << name() << " new, cmd=" << (cmd ? 'W' : 'R')
                  << ", data=" << hex << data[i % 16] << " at time " << sc_time_stamp() << endl;
        */

        // Non-blocking transport call on the forward path
        tlm::tlm_sync_enum status;
        status = init_socket->nb_transport_fw( *trans, phase, delay );

        // Check value returned from nb_transport_fw
        if (status == tlm::TLM_UPDATED) {

            // The timing annotation must be honored
            m_payload_event_queue.notify( *trans, phase, delay );
        }
        else if (status == tlm::TLM_COMPLETED) {

            // The completion of the transaction necessarily ends the BEGIN_REQ phase
            request_in_progress = 0;

            // The target has terminated the transaction
            check_transaction( *trans );
        }
        wait(sc_time(rand_ps(), SC_PS));
    }
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
    // The timing annotation must be honored
    //m_peq.notify( trans, phase, delay );
    std::cout << "Message received! CPU: " << cpu_id << " Socket: " << init_socket.name() << " " << hex << trans.get_address()
              << " " << name() << " new, cmd=" << (trans.get_command() ? 'W' : 'R') << endl;

    m_payload_event_queue.notify(trans, phase, delay);

    return tlm::TLM_ACCEPTED;
}

void CPU::check_transaction(tlm::tlm_generic_payload& trans)
{
    if ( trans.is_response_error() )
    {
        char txt[100];
        sprintf(txt, "Transaction returned with error, response status = %s",
        trans.get_response_string().c_str());
        SC_REPORT_ERROR("TLM-2", txt);
    }

    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    int*             ptr = reinterpret_cast<int*>( trans.get_data_ptr() );

    std::cout << hex << adr << " " << name() << " check, cmd=" << (cmd ? 'W' : 'R')
              << ", data=" << hex << *ptr << " at time " << sc_time_stamp() << endl;

    // Allow the memory manager to free the transaction object
    trans.release();
}

void CPU::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
{

    if (phase == tlm::END_REQ) {
        std::cout << hex << trans.get_address() << " " << name() << " END_REQ at " << sc_time_stamp() << endl;
    }
    else if (phase == tlm::BEGIN_RESP) {
        std::cout << hex << trans.get_address() << " " << name() << " BEGIN_RESP at " << sc_time_stamp() << endl;
    }

    if (phase == tlm::END_REQ || (&trans == request_in_progress && phase == tlm::BEGIN_RESP))
    {
        // The end of the BEGIN_REQ phase
        request_in_progress = NULL;
        end_request_event.notify();
    }
    else if (phase == tlm::BEGIN_REQ || phase == tlm::END_RESP) {

        SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");
    }

    if (phase == tlm::BEGIN_RESP) {

        check_transaction( trans );

        // Send final phase transition to target
        tlm::tlm_phase fw_phase = tlm::END_RESP;
        sc_time delay = sc_time(rand_ps(), SC_PS);
        init_socket->nb_transport_fw( trans, fw_phase, delay );
        // Ignore return value
    }
}
