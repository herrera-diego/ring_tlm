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


#define ID_MASK             0xFF000000
#define SRC_MASK            0xFF0000
#define DST_MASK            0xFF00
#define ADDR_MASK           0xFF

#define ID_SHIFT            24
#define SRC_SHIFT           16
#define DST_SHIFT           8
#define ADDR_SHIFT          0

inline unsigned int decode_address(sc_dt::uint64 address, sc_dt::uint64& masked_address)
{
    unsigned int target_nr = static_cast<unsigned int>( address & 0x1 );
    masked_address = address;
    return target_nr;
}

inline sc_dt::uint64 compose_address(unsigned int trans_id, unsigned int src, unsigned int dst, unsigned int addr)
{
    unsigned int address = (((trans_id << ID_SHIFT) & ID_MASK) |
                            ((src << SRC_SHIFT) & SRC_MASK) |
                            ((dst << DST_SHIFT) & DST_MASK) |
                            (addr & ADDR_MASK));
    return address;
}

inline unsigned int decode_addr(sc_dt::uint64 address)
{
    return (address & ADDR_MASK);
}

inline unsigned int decode_transID(sc_dt::uint64 address)
{
    return ((address >> ID_SHIFT) & ADDR_MASK);
}

inline unsigned int decode_dest(sc_dt::uint64 address)
{
    return ((address >> DST_SHIFT) & ADDR_MASK);
}

inline unsigned int decode_src(sc_dt::uint64 address)
{
    return ((address >> SRC_SHIFT) & ADDR_MASK);
}

#endif //__CONSTANTS_H__
