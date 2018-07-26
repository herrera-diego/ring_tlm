#ifndef CPU_H
#define CPU_H

#include "router.h"

using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
class ComputerNode: public Router   
{   
    public:
        // Internal data buffer used by initiator with generic payload   
        int data; 

        void thread_process();
        
        //ComputerNode(sc_module_name name) : Router(name)
        SC_CTOR(ComputerNode) : Router("Router")
        //: socket_initiator("socket")  // Construct and name socket   
        {   
            // Register callbacks for incoming interface method calls
            this->socket_initiator.register_nb_transport_bw(this, &ComputerNode::nb_transport_bw);
            this->socket_target.register_nb_transport_fw(this, &ComputerNode::nb_transport_fw);
            
            SC_THREAD(thread_process);   
        }    
};

#endif
