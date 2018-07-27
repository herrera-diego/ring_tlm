#ifndef MEMORY_H
#define MEMORY_H

#include "ID_Extension.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   

class Memory: public sc_module
{   
    public:
        enum { SIZE = 256 };   
        const sc_time LATENCY;   

        int mem[SIZE];   

        tlm_utils::simple_target_socket<Memory> socket_target;
        tlm::tlm_generic_payload* trans_pending;   
        tlm::tlm_phase phase_pending;   
        sc_time delay_pending;

        sc_event  e1;
        
        // *********************************************   
        // Thread to call nb_transport on backward path   
        // ********************************************* 
        void thread_process();
        void readMem();
        
        SC_CTOR(Memory)   
        :socket_target("socket"), LATENCY(10, SC_NS)   
        {   
            readMem();       
            SC_THREAD(thread_process);   
        }   
};   
#endif
