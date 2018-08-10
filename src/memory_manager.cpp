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

#include "memory_manager.h"

memory_manager::memory_manager()
{
    free_list = NULL;
    empties = NULL;
}

tlm::tlm_generic_payload* memory_manager::memory_manager::allocate()
{
    tlm::tlm_generic_payload* ptr;

    if (free_list) {
        ptr = free_list->trans;
        empties = free_list;
        free_list = free_list->next;
    }
    else {
        ptr = new tlm::tlm_generic_payload(this);
    }

    return ptr;
}

void memory_manager::free(tlm::tlm_generic_payload* trans)
{
    if (!empties) {
        empties = new mm_message;
        empties->next = free_list;
        empties->prev = NULL;
        
        if (free_list) {
            free_list->prev = empties;
        }
    }

    free_list = empties;
    free_list->trans = trans;
    empties = free_list->prev;
}
