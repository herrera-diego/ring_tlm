#ifndef TOP_H
#define TOP_H

#include "cpu.h"
#include "memory.h"
#include "router.h"
#include "RouterEvents.h"

#define MAX_NUM_CPU     3
#define MAX_NUM_ROUTER  4

SC_MODULE(Top)
{
    CPU             *cpu[MAX_NUM_CPU];
    Memory          *memory;
    Router          *router[MAX_NUM_ROUTER];

    static RouterEvents RouterEvent;

    SC_CTOR(Top)
    {
        // Instantiate components
        for (int i = 0; i < MAX_NUM_CPU; i++) {
            char txt[10];
            sprintf(txt, "CPU_%d", i);

            cpu[i] = new CPU(txt, i);
        }
    
        for (int i = 0; i < MAX_NUM_ROUTER; i++) {
            char txt[10];
            sprintf(txt, "Router_%d", i);

            router[i] = new Router(txt, i);
        }

        //initiator2 = new CPU("initiator2");
        memory    = new Memory("memory");

        cpu[0]->init_socket.bind(router[0]->target_socket);
        cpu[1]->init_socket.bind(router[1]->target_socket);
        cpu[2]->init_socket.bind(router[2]->target_socket);

        router[0]->init_socket.bind(router[1]->target_socket);
        router[1]->init_socket.bind(router[2]->target_socket);
        router[2]->init_socket.bind(router[3]->target_socket);
        router[3]->init_socket.bind(router[0]->target_socket);

        // Path to Memory
        router[3]->init_socket.bind(memory->socket_target);
    }
};   
#endif
