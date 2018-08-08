#ifndef TOP_H
#define TOP_H

#include "cpu.h"
#include "memory.h"
#include "router.h"

SC_MODULE(Top)
{
    CPU             *initiator;
    //CPU           *initiator2;
    Memory          *memory;

    Router          *router0;
    // Router       *router1; 
    // Router       *router2;

    sc_event *my_event_ptr; 


    SC_CTOR(Top)
    {
        // Instantiate components
         my_event_ptr = new sc_event; 

        router0 = new Router("R0", my_event_ptr);

        initiator = new CPU("cpu0", my_event_ptr);   
        //initiator2 = new CPU("initiator2");
        memory    = new Memory   ("memory", my_event_ptr);
        //memory->my_event_ptr(my_event_ptr);   

        // router1 = new Router("R1");
        // router2 = new Router("R2");
        //tlm_utils::simple_target_socket<Router> mysocket;
   
        // One initiator is bound directly to one target with no intervening bus
   
        // Bind initiator socket to target socket
        //initiator2->socket_initiator.bind(initiator->socket_target);

        //  initiator->cpuRouter.socket_initiator.bind(router0->socket_target);
        // router0->socket_initiator.bind(memory->memRouter.socket_target);
        // memory->memRouter.socket_initiator.bind(initiator->cpuRouter.socket_target);

        initiator->socket_initiator.bind(router0->socket_target);
        router0->socket_initiator.bind(memory->socket_target);
        

         //initiator->socket_initiator.bind(memory->socket_target);

        // initiator->cpuRouter.socket_initiator.bind(memory->memRouter.socket_target);
        // memory->memRouter.socket_initiator.bind(initiator->cpuRouter.socket_target);


        //router1->socket_initiator.bind(router2->socket_target);
        //router2->socket_initiator.bind(memory->socket_target);
        //memory->socket_initiator.bind(initiator->socket_target);
        //initiator->socket_target.bind(mysocket);
    }
};   
#endif
