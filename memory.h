#ifndef MEMORY_H
#define MEMORY_H

#include "router.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   

class Memory: public Router
{   
    public:
        enum { SIZE = 256 };   
        const sc_time LATENCY;   

        int mem[SIZE];   
        
        // *********************************************   
        // Thread to call nb_transport on backward path   
        // ********************************************* 
        void thread_process();
        
        SC_CTOR(Memory)   
        : Router("Router"), LATENCY(10, SC_NS)   
        {   
            // Register callbacks for incoming interface method calls
            this->socket_target.register_nb_transport_fw(this, &Memory::nb_transport_fw);
            this->socket_initiator.register_nb_transport_bw(this, &Memory::nb_transport_bw);
        
            // Initialize memory with random data   
            for (int i = 0; i < SIZE; i++)   
            mem[i] = 0xAA000000 | (rand() % 256);   
        
            SC_THREAD(thread_process);   
        }   
};   
#endif
