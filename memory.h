#ifndef MEMORY_H
#define MEMORY_H

#include <systemc.h>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "ID_Extension.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   

class Memory: sc_module   
{   
    public:
        // TLM-2 socket, defaults to 32-bits wide, base protocol
        tlm_utils::simple_target_socket<Memory> socket;
        
        enum { SIZE = 256 };   
        const sc_time LATENCY;   

        int mem[SIZE];   
        sc_event  e1;
        tlm::tlm_generic_payload* trans_pending;   
        tlm::tlm_phase phase_pending;   
        sc_time delay_pending;

        // TLM2 non-blocking transport method     
        virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                                    tlm::tlm_phase& phase, sc_time& delay );
        
        // *********************************************   
        // Thread to call nb_transport on backward path   
        // ********************************************* 
        void thread_process();
        
        SC_CTOR(Memory)   
        : socket("socket"), LATENCY(10, SC_NS)   
        {   
            // Register callbacks for incoming interface method calls
            socket.register_nb_transport_fw(this, &Memory::nb_transport_fw);
            //socket.register_nb_transport_bw(this, &Memory::nb_transport_bw);
        
            // Initialize memory with random data   
            for (int i = 0; i < SIZE; i++)   
            mem[i] = 0xAA000000 | (rand() % 256);   
        
            SC_THREAD(thread_process);   
        }   
};   
#endif
