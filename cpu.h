#ifndef CPU_H
#define CPU_H

#include "ID_Extension.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
class ComputerNode: public sc_module   
{   
    public:
        // Internal data buffer used by initiator with generic payload   
        int data; 
        const char* routerName;
        void thread_process();

        tlm_utils::simple_initiator_socket<ComputerNode> socket_initiator;
        tlm::tlm_generic_payload* trans_pending;   
        tlm::tlm_phase phase_pending;   
        sc_time delay_pending;
        
        //ComputerNode(sc_module_name name) : Router(name)
        SC_CTOR(ComputerNode) : socket_initiator("socket")
        //: socket_initiator("socket")  // Construct and name socket   
        {   
            SC_THREAD(thread_process);   
        }    
};

#endif
