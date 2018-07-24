#ifndef CPU_H
#define CPU_H

#include "router.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
class ComputerNode: Router   
{   
    public:
        // Internal data buffer used by initiator with generic payload   
        int data; 

        void thread_process();
        
        SC_CTOR(ComputerNode)   
        //: socket_initiator("socket")  // Construct and name socket   
        {   
            // Register callbacks for incoming interface method calls
            this->socket_initiator.register_nb_transport_bw(this, &ComputerNode::nb_transport_bw);
            
            SC_THREAD(thread_process);   
        }    
};

#endif
