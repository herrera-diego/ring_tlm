#ifndef ROUTER_H
#define ROUTER_H

#include "ID_Extension.h"
#include "RouterEvents.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
class Router: public sc_module   
{       
    public:
        // TLM2 non-blocking transport method     
        virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                                    tlm::tlm_phase& phase, sc_time& delay );
        
        virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,   
                                                    tlm::tlm_phase& phase, sc_time& delay ) ;
 
        
        // Internal data buffer used by initiator with generic payload   
        int data; 

        // TLM2 socket, defaults to 32-bits wide, generic payload, generic DMI mode   
        tlm_utils::simple_initiator_socket<Router> socket_initiator; 
        tlm_utils::simple_target_socket<Router> socket_target;
        
        const sc_time LATENCY;   
  
        //sc_event  e1;
        tlm::tlm_generic_payload* trans_pending;   
        tlm::tlm_phase phase_pending;   
        sc_time delay_pending;
        
        SC_CTOR(Router)   
        : socket_initiator("Initiator"),  socket_target("Target"), LATENCY(10, SC_NS)   
        {   
            // Register callbacks for incoming interface method calls
            socket_target.register_nb_transport_fw(this, &Router::nb_transport_fw);
            socket_initiator.register_nb_transport_bw(this, &Router::nb_transport_bw);     
        }   
};

#endif
