#ifndef CPU_H
#define CPU_H

#include <systemc.h>

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

class CPU: public sc_module
{
    public:
        // Internal data buffer used by initiator with generic payload
        int                                     data;
        const char                              *routerName;

        tlm_utils::simple_initiator_socket<CPU> socket_initiator;
        tlm::tlm_generic_payload                *trans_pending;
        tlm::tlm_phase                          phase_pending;
        sc_time                                 delay_pending;
                
        void thread_process();

        CPU(sc_module_name name_) ;
        
        SC_HAS_PROCESS(CPU);    

};

#endif
