#ifndef CPU_H
#define CPU_H

#include <systemc.h>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "ID_Extension.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
class Initiator: sc_module   
{   
    // Internal data buffer used by initiator with generic payload   
    int data; 

    // TLM2 socket, defaults to 32-bits wide, generic payload, generic DMI mode   
    tlm_utils::simple_initiator_socket<Initiator> socket; 


    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,   
                                        tlm::tlm_phase& phase, sc_time& delay ) ;

    void thread_process();
    
    SC_CTOR(Initiator)   
    : socket("socket")  // Construct and name socket   
    {   
        // Register callbacks for incoming interface method calls
        socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);
        
        SC_THREAD(thread_process);   
    }    
};

#endif
