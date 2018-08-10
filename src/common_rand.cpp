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

#include "systemc.h"

#include "common_rand.h"

// Generate a random delay (with power-law distribution) to aid testing and stress the protocol
int rand_ps()
{
    int n = rand() % 100;
    n = n * n * n;
    return n/100;
}

