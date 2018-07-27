#include "router.h"

// *********************************************   
// TLM2 backward path non-blocking transport method   
// *********************************************   

tlm::tlm_sync_enum Router::nb_transport_bw( tlm::tlm_generic_payload& trans,   
                                        tlm::tlm_phase& phase, sc_time& delay )   
{   
    tlm::tlm_command cmd = trans.get_command();   
    sc_dt::uint64    adr = trans.get_address();   

    ID_extension* id_extension = new ID_extension;
    trans.get_extension( id_extension ); 

    if (phase == tlm::END_RESP) {  
            
        //Delay for TLM_COMPLETE
        wait(delay);
        
        cout << name() << " END_RESP RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
            
        return tlm::TLM_COMPLETED;
        
    }

    if (phase == tlm::BEGIN_RESP) {
                                
        // Initiator obliged to check response status   
        if (trans.is_response_error() )   
        SC_REPORT_ERROR("TLM2", "Response error from nb_transport");   
            
        cout << "trans/bw = { " << (cmd ? 'W' : 'R') << ", " << hex << adr   
            << " } , data = " << hex << data << " at time " << sc_time_stamp()   
            << ", delay = " << delay << endl;
        
        //Delay
        wait(delay);
        
        cout << name () << " BEGIN_RESP RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
        
        return tlm::TLM_ACCEPTED;   
    }   
}   

tlm::tlm_sync_enum Router::nb_transport_fw( tlm::tlm_generic_payload& trans,
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
  