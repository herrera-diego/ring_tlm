#ifndef TOP_H
#define TOP_H

#include "cpu.h"
#include "memory.h"

SC_MODULE(Top)   
{   
  ComputerNode *initiator;   
  ComputerNode *initiator2;   
  Memory    *memory;   
   
  SC_CTOR(Top)   
  {   
    // Instantiate components   
    initiator = new ComputerNode("initiator");  
    initiator2 = new ComputerNode("initiator2");  
    memory    = new Memory   ("memory");   
    //tlm_utils::simple_target_socket<Router> mysocket;
   
    // One initiator is bound directly to one target with no intervening bus   
   
    // Bind initiator socket to target socket  
    initiator2->socket_initiator.bind(initiator->socket_target); 
    initiator->socket_initiator.bind(memory->socket_target); 
    memory->socket_initiator.bind(initiator2->socket_target); 
    //initiator->socket_target.bind(mysocket);   
  }   
};   
#endif
