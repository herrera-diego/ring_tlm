  
#include <systemc.h>   
using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "ID_Extension.h"


// Initiator module generating generic payload transactions   
   
class Initiator: sc_module   
{   
  // TLM2 socket, defaults to 32-bits wide, generic payload, generic DMI mode   
  tlm_utils::simple_initiator_socket<Initiator> socket; 
   
  SC_CTOR(Initiator)   
  : socket("socket")  // Construct and name socket   
  {   
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);
    
    SC_THREAD(thread_process);   
  }   
   
  void thread_process()   
  {   
    // TLM2 generic payload transaction
    tlm::tlm_generic_payload trans;
    ID_extension* id_extension = new ID_extension;
    //trans.set_extension( id_extension ); // Add the extension to the transaction
    trans.set_extension( id_extension ); // Add the extension to the transaction
     
    // Generate a random sequence of reads and writes   
    for (int i = 0; i < 100; i++)   
    {   
      tlm::tlm_phase phase = tlm::BEGIN_REQ;   
      sc_time delay = sc_time(10, SC_NS);   
            
      tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);   
      trans.set_command( cmd );   
      trans.set_address( rand() % 0xFF );   
      if (cmd == tlm::TLM_WRITE_COMMAND) data = 0xFF000000 | i;   
      trans.set_data_ptr( reinterpret_cast<unsigned char*>(&data) );   
      trans.set_data_length( 4 );   
   
      // Other fields default: byte enable = 0, streaming width = 0, DMI_hint = false, no extensions   
      
      //Delay for BEGIN_REQ
      wait(10, SC_NS);
      tlm::tlm_sync_enum status;   
      
      cout << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
      status = socket->nb_transport_fw( trans, phase, delay );  // Non-blocking transport call   
   
      // Check value returned from nb_transport   
   
      switch (status)   
      {   
      case tlm::TLM_ACCEPTED:   
        
        //Delay for END_REQ
        wait(10, SC_NS);
        
        cout << name() << " END_REQ SENT" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
        // Expect response on the backward path  
        phase = tlm::END_REQ; 
        status = socket->nb_transport_fw( trans, phase, delay );  // Non-blocking transport call
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
   
  // *********************************************   
  // TLM2 backward path non-blocking transport method   
  // *********************************************   
   
  virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,   
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
  // Internal data buffer used by initiator with generic payload   
  int data;   
};   
   