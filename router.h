#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

class Router: public sc_core::sc_module
{
    public:
        // TLM2 non-blocking transport method
        virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                                    tlm::tlm_phase& phase, sc_time& delay );
        
        virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,
                                                    tlm::tlm_phase& phase, sc_time& delay ) ;
 
 
        // Internal data buffer used by initiator with generic payload
        int 						                data;

        // TLM2 socket, defaults to 32-bits wide, generic payload, generic DMI mode
        tlm_utils::simple_initiator_socket<Router> 	socket_initiator;
        tlm_utils::simple_target_socket<Router> 	socket_target;
        
        const sc_time 					            LATENCY;
  
        //sc_event  e1;
        //sc_event  procEvent;
        tlm::tlm_generic_payload 			        *trans_pending;
        tlm::tlm_phase                              phase_pending;
        sc_time                                     delay_pending;
        
        Router(sc_module_name name_, sc_event * e) ;
        
        SC_HAS_PROCESS(Router);      

    //private:
        sc_event * my_event_ptr;
        
};

#endif
