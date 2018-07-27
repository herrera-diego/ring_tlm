#ifndef TOP_H
#define TOP_H

#include "cpu.h"
#include "memory.h"
#include "router.h"

SC_MODULE(Top)   
{   
  ComputerNode *initiator;   
  //ComputerNode *initiator2;   
  Memory    *memory;  

  Router *router0;
  // Router *router1; 
  // Router *router2; 
  void syncEvents()
  {
    wait(router0->e1);

    memory->e1.notify();
  }

  SC_CTOR(Top)   
  {   
    // Instantiate components   
    initiator = new ComputerNode("initiator");
    
    router0 = new Router("R0");
    //initiator2 = new ComputerNode("initiator2");  
    memory    = new Memory   ("memory");   

    SC_THREAD(syncEvents);
    // router1 = new Router("R1");
    // router2 = new Router("R2");
    //tlm_utils::simple_target_socket<Router> mysocket;
   
    // One initiator is bound directly to one target with no intervening bus   
   
    // Bind initiator socket to target socket  
    //initiator2->socket_initiator.bind(initiator->socket_target); 
    initiator->socket_initiator.bind(router0->socket_target); 
    router0->socket_initiator.bind(memory->socket_target); 
    //router1->socket_initiator.bind(router2->socket_target);
    //router2->socket_initiator.bind(memory->socket_target);
    //memory->socket_initiator.bind(initiator->socket_target); 
    //initiator->socket_target.bind(mysocket);   
  }   
};   
#endif
