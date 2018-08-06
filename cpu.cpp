#include "cpu.h"
#include "ID_Extension.h"

CPU::CPU(sc_core::sc_module_name name_) : socket_initiator("socket")
{   
    SC_THREAD(thread_process);   
} 

void CPU::thread_process()   
{

    for (int i = 0; i < 5; i++)   
    {
        std::cout << "Running thread of CPU: " << routerName << "\n";
    }

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
}   
