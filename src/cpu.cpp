#include "cpu.h"
#include "ID_Extension.h"

CPU::CPU(sc_core::sc_module_name module_name, unsigned int id) : sc_module(module_name), init_socket("init_socket")
{
    cpu_id = id;

    init_socket.register_nb_transport_bw(this, &CPU::nb_transport_bw);

    SC_THREAD(thread_process);
}

void CPU::thread_process()
{
    //if (cpu_id == 0) {
        std::cout << "Running thread of CPU: " << cpu_id << "\n";

        int i = 0;
        tlm::tlm_generic_payload* trans;

        tlm::tlm_phase phase = tlm::BEGIN_REQ;   
        sc_time delay = sc_time(10, SC_NS);

        int adr = rand();
        tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);

        if (cmd == tlm::TLM_WRITE_COMMAND)
            data[i % 16] = rand();

        // Grab a new transaction from the memory manager
        trans = m_mm.allocate();
        trans->acquire();

        // Set all attributes except byte_enable_length and extensions (unused)
        trans->set_command( cmd );
        trans->set_address( adr );
        trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data[i % 16]) );
        trans->set_data_length( 4 );
        trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
        trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
        trans->set_dmi_allowed( false ); // Mandatory initial value
        trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

        std::cout << "Message from CPU: " << cpu_id << " Socket: " << init_socket.name() << " " << hex << adr << " " << name() << " new, cmd=" << (cmd ? 'W' : 'R') << ", data=" << hex << data[i % 16] << " at time " << sc_time_stamp() << endl;

        tlm::tlm_sync_enum status = init_socket->nb_transport_fw(*trans, phase, delay);
        //wait(100, SC_NS);
    //}
    //else {
    //    std::cout << "Running thread of non valid CPU\n";
    //}

#if 0

    // TLM2 generic payload transaction
    tlm::tlm_generic_payload trans;
    ID_extension* id_extension = new ID_extension;
    //trans.set_extension( id_extension ); // Add the extension to the transaction
    trans.set_extension( id_extension ); // Add the extension to the transaction
        
    // Generate a random sequence of reads and writes   
    for (int i = 0; i < 5; i++)   
    {   
        tlm::tlm_phase phase = tlm::BEGIN_REQ;   
        sc_time delay = sc_time(10, SC_NS);   
            
        tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);   
        trans.set_command( cmd );   
        trans.set_address( rand() % 0xFF );   
        if (cmd == tlm::TLM_WRITE_COMMAND)
        { 
            data = 0xFF000000 | i;   
        }
        trans.set_data_ptr( reinterpret_cast<unsigned char*>(&data) );   
        trans.set_data_length( 4 );   

        // Other fields default: byte enable = 0, streaming width = 0, DMI_hint = false, no extensions   
        
        //Delay for BEGIN_REQ
        wait(10, SC_NS);
        tlm::tlm_sync_enum status;   
        
        cout << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
        status = socket_initiator->nb_transport_fw( trans, phase, delay );  // Non-blocking transport call   

        // Check value returned from nb_transport   

        switch (status)   
        {   
            case tlm::TLM_ACCEPTED:   
            
                //Delay for END_REQ
                wait(10, SC_NS);
                
                cout << name() << " END_REQ SENT" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
                // Expect response on the backward path  
                phase = tlm::END_REQ; 
                status = socket_initiator->nb_transport_fw( trans, phase, delay );  // Non-blocking transport call
                break;   

            case tlm::TLM_UPDATED:   
            case tlm::TLM_COMPLETED:   

                // Initiator obliged to check response status   
                if (trans.is_response_error() )   
                    SC_REPORT_ERROR("TLM2", "Response error from nb_transport_fw");   

                cout << "trans/fw = { " << (cmd ? 'W' : 'R') << ", " << hex << i << " } , data = "   
                        << hex << data << " at time " << sc_time_stamp() << ", delay = " << delay << endl;   
                break;   
        }
        
        //Delay between RD/WR request
        wait(100, SC_NS);
        
        id_extension->transaction_id++; 
    }
#endif
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
    // The timing annotation must be honored
    //m_peq.notify( trans, phase, delay );
    std::cout << "Message received! CPU: " << cpu_id << " Socket: " << init_socket.name() << " " << hex << trans.get_address()
        << " " << name() << " new, cmd=" << (trans.get_command() ? 'W' : 'R') << endl;

    return tlm::TLM_ACCEPTED;
}
