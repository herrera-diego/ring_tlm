#ifndef MEMORY_H
#define MEMORY_H

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "router.h"

#define SIZE 256

class Memory: public sc_module
{   
    public:
        const sc_time                           LATENCY;

        int                                     mem[SIZE];

        Router                                  memRouter;
        sc_event *my_event_ptr; 
        
        // *********************************************   
        // Thread to call nb_transport on backward path   
        // ********************************************* 
        void thread_process();
        void readMem();
        
            
        Memory(sc_module_name name_, sc_event * e) ;
        
        SC_HAS_PROCESS(Memory);      
};   
#endif
