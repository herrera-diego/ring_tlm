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