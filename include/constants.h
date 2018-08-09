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

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "systemc.h"

#define MAX_NUMBER_NODES    8
#define TOP_ROUTER          7
#define MAX_NUM_CPU         (MAX_NUMBER_NODES - 1)
#define MAX_NUM_ROUTER      MAX_NUMBER_NODES

#define MEMORY_SIZE         256

#define NUM_TRANSACTIONS    1000

inline unsigned int decode_address(sc_dt::uint64 address, sc_dt::uint64& masked_address)
{
    unsigned int target_nr = static_cast<unsigned int>( address & 0x1 );
    masked_address = address;
    return target_nr;
}

inline sc_dt::uint64 compose_address(unsigned int target_nr, sc_dt::uint64 address)
{
    return address;
}

#endif //__CONSTANTS_H__

