/*******************************************************
 *  Instituto Tecnologico de Costa Rica
 *
 *  Diseño de Alto Nivel de Sistema Electrónicos
 *
 *  Proyecto 2 (Grupo 1)
 *
 *  Mario Castro    200827325
 *  Diego Herrera   200324558
 *  Fernando París  200510153
 *  Esteban Rivera  2018319491
 *  Kevin Víquez    200944341
 *******************************************************/

#ifndef __TOP_H__
#define __TOP_H__

#include "systemc.h"

#include "constants.h"
#include "cpu.h"
#include "memory.h"
#include "router.h"

SC_MODULE(Top)
{
    CPU             *cpu[MAX_NUM_CPU];
    Memory          *memory;
    Router          *router[MAX_NUM_ROUTER];

    SC_CTOR(Top)
    {
        // Memory initialization
        memory    = new Memory("Memory");

        // Instantiate components
        for (int i = 0; i < MAX_NUM_CPU; i++) {

            char txt[10];
            sprintf(txt, "CPU_%d", i);

            std::cout << "Creating: " << txt << "\n";

            cpu[i] = new CPU(txt, i);
        }
    
        for (int i = 0; i < MAX_NUM_ROUTER - 1; i++) {

            char txt[10];
            sprintf(txt, "Router_%d", i);

            std::cout << "Creating: " << txt << "\n";

            router[i] = new Router(txt, i);
        }

        // Create Router 7 (Always has to be present!)
        router[MAX_NUM_ROUTER - 1] = new Router("Router_7", TOP_ROUTER);

        for (int i = 0; i < MAX_NUM_CPU; i++) {
            cpu[i]->init_socket.bind(router[i]->target_socket);
        }

        // Ring Connectivity
        for (int i = 0; i < MAX_NUM_ROUTER; i++) {

            if (i == (MAX_NUM_ROUTER - 1)) {
                router[i]->init_socket.bind(router[0]->target_socket);
            }
            else {
                router[i]->init_socket.bind(router[i+1]->target_socket);
            }
        }

        // Path to Memory
        router[MAX_NUM_ROUTER - 1]->init_socket.bind(memory->target_socket);
    }
};   
#endif //__TOP_H__

