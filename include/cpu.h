#ifndef CPU_H
#define CPU_H

#include <systemc.h>

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "memory_manager.h"

class CPU: public sc_module
{
    public:
        // Internal data buffer used by initiator with generic payload
        tlm_utils::simple_initiator_socket<CPU> init_socket;

        // This should be used instead of SC_CTOR to enable a custom constructor
        SC_HAS_PROCESS(CPU);
        CPU(sc_core::sc_module_name module_name, unsigned int id);
        void thread_process();

        virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay);

        memory_manager                          m_mm;
        tlm::tlm_generic_payload                *request_in_progress;
        int                                     data[16];
        int                                     cpu_id;
};

#endif
