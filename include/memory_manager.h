#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

#include "tlm.h"

class memory_manager: public tlm::tlm_mm_interface
{
    public:
        memory_manager();

        tlm::tlm_generic_payload*       allocate();
        void                            free(tlm::tlm_generic_payload* trans);

    private:

        struct mm_message
        {
            tlm::tlm_generic_payload*   trans;
            mm_message*                 next;
            mm_message*                 prev;
        };

        mm_message*                     free_list;
        mm_message*                     empties;
};

#endif