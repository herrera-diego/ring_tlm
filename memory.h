#ifndef MEMORY_H
#define MEMORY_H

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#define SIZE 256

class Memory: public sc_module
{   
    public:
        const sc_time                           LATENCY;

        int                                     mem[SIZE];

        tlm_utils::simple_target_socket<Memory> socket_target;
        tlm::tlm_generic_payload                *trans_pending;
        tlm::tlm_phase                          phase_pending;
        sc_time                                 delay_pending;
        
        // *********************************************   
        // Thread to call nb_transport on backward path   
        // ********************************************* 
        void thread_process();
        void readMem();
        
        SC_CTOR(Memory);
};   
#endif
