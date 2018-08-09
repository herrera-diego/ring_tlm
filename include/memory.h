#ifndef MEMORY_H
#define MEMORY_H

#include <queue>

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "constants.h"

DECLARE_EXTENDED_PHASE(internal_ph);

class Memory: public sc_module
{   
    public:
        // *********************************************
        // Constructor and Methods
        // *********************************************
        SC_CTOR(Memory);

        void init_memory();

        virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                                    tlm::tlm_phase& phase, sc_time& delay );

        void peq_cb(                                tlm::tlm_generic_payload& trans,
                                                    const tlm::tlm_phase& phase);

        tlm::tlm_sync_enum send_end_req(            tlm::tlm_generic_payload& trans);

        void send_response(                         tlm::tlm_generic_payload& trans);

        // *********************************************
        // Class attributes
        // *********************************************
        tlm_utils::simple_target_socket<Memory>     target_socket;

        tlm::tlm_generic_payload                    *next_response_pending;
        std::queue<tlm::tlm_generic_payload*>       end_req_pending;
        tlm_utils::peq_with_cb_and_phase<Memory>    m_payload_event_queue;

        int                                         mem[MEMORY_SIZE];
        int                                         n_trans;

        bool                                        response_in_progress;
};   
#endif
