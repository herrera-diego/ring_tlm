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

#ifndef __CPU_H__
#define __CPU_H__

#include <systemc.h>

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "memory_manager.h"

class CPU: public sc_module
{
    public:
        // This should be used instead of SC_CTOR to enable a custom constructor
        SC_HAS_PROCESS(CPU);

        CPU(sc_core::sc_module_name module_name, unsigned int id);

        void thread_process();

        virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,
                                                    tlm::tlm_phase& phase, sc_time& delay);

        void check_transaction(                     tlm::tlm_generic_payload& trans);

        void peq_cb(                                tlm::tlm_generic_payload& trans,
                                                    const tlm::tlm_phase& phase);

        // Internal data buffer used by initiator with generic payload
        tlm_utils::simple_initiator_socket<CPU>     init_socket;
        memory_manager                              m_mm;
        tlm::tlm_generic_payload                    *request_in_progress;
        tlm_utils::peq_with_cb_and_phase<CPU>       m_payload_event_queue;
        sc_event                                    end_request_event;

        int                                         data;
        int                                         cpu_id;
};

#endif  // __CPU_H__

