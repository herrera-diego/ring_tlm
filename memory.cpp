#include "memory.h"

tlm::tlm_sync_enum Memory::nb_transport_fw( tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
{
    sc_dt::uint64    adr = trans.get_address();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();

    ID_extension* id_extension = new ID_extension;
    trans.get_extension( id_extension ); 
    
    if(phase == tlm::END_REQ){
      
      wait(delay);
      
      cout << name() << " END_REQ RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
      
      return tlm::TLM_COMPLETED;
    }
    if(phase == tlm::BEGIN_REQ){
      // Obliged to check the transaction attributes for unsupported features
      // and to generate the appropriate error response
      if (byt != 0) {
        trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
        return tlm::TLM_COMPLETED;
      }
      //if (len > 4 || wid < len) {
      //  trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
      //  return tlm::TLM_COMPLETED;
      //}

      // Now queue the transaction until the annotated time has elapsed
      trans_pending=&trans;
      phase_pending=phase;
      delay_pending=delay;

      e1.notify();
      
      //Delay
      wait(delay);
      
      cout << name() << " BEGIN_REQ RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;      
      
      return tlm::TLM_ACCEPTED;
    }  
}
  
void Memory::thread_process()  
{   
    while (true) {

        // Wait for an event to pop out of the back end of the queue   
        wait(e1); 
        //printf("ACCESING MEMORY\n");

        //tlm::tlm_generic_payload* trans_ptr;   
        tlm::tlm_phase phase;   
        
        ID_extension* id_extension = new ID_extension;
        trans_pending->get_extension( id_extension ); 
        
        tlm::tlm_command cmd = trans_pending->get_command();   
        sc_dt::uint64    adr = trans_pending->get_address() / 4;   
        unsigned char*   ptr = trans_pending->get_data_ptr();   
        unsigned int     len = trans_pending->get_data_length();   
        unsigned char*   byt = trans_pending->get_byte_enable_ptr();   
        unsigned int     wid = trans_pending->get_streaming_width();   

        // Obliged to check address range and check for unsupported features,   
        //   i.e. byte enables, streaming, and bursts   
        // Can ignore DMI hint and extensions   
        // Using the SystemC report handler is an acceptable way of signalling an error   
        
        if (adr >= sc_dt::uint64(SIZE) || byt != 0 || wid != 0 || len > 4)   
        SC_REPORT_ERROR("TLM2", "Target does not support given generic payload transaction");   
        
        // Obliged to implement read and write commands   
        if ( cmd == tlm::TLM_READ_COMMAND )   
        memcpy(ptr, &mem[adr], len);   
        else if ( cmd == tlm::TLM_WRITE_COMMAND )   
        memcpy(&mem[adr], ptr, len);   
                
        // Obliged to set response status to indicate successful completion   
        trans_pending->set_response_status( tlm::TLM_OK_RESPONSE );  
        
        wait(20, SC_NS);
        
        delay_pending= (rand() % 4) * sc_time(10, SC_NS);
        
        cout << name() << " BEGIN_RESP SENT" << " TRANS ID " << id_extension->transaction_id <<  " at time " << sc_time_stamp() << endl;
        
        // Call on backward path to complete the transaction
        tlm::tlm_sync_enum status;
        phase = tlm::BEGIN_RESP;   
        status = socket->nb_transport_bw( *trans_pending, phase, delay_pending );   

        // The target gets a final chance to read or update the transaction object at this point.   
        // Once this process yields, the target must assume that the transaction object   
        // will be deleted by the initiator   

        // Check value returned from nb_transport   

        switch (status)   
        
        //case tlm::TLM_REJECTED:   
        case tlm::TLM_ACCEPTED:   
            
            wait(10, SC_NS);
            
            cout << name() << " END_RESP SENT" << " TRANS ID " << id_extension->transaction_id <<  " at time " << sc_time_stamp() << endl;
            // Expect response on the backward path  
            phase = tlm::END_RESP; 
            socket->nb_transport_bw( *trans_pending, phase, delay_pending );  // Non-blocking transport call
        //break;           
    }   
} 