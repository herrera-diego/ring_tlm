#ifndef CPU_H
#define CPU_H

#include <systemc.h>

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "router.h"

class CPU: public sc_module
{
    public:
        // Internal data buffer used by initiator with generic payload
        int                                     data;
        const char                              *routerName;

        Router                                  cpuRouter;
        sc_event *my_event_ptr; 
                
        void thread_process();

        CPU(sc_module_name name_, sc_event * e) ;
        
        SC_HAS_PROCESS(CPU);    

};

#endif
