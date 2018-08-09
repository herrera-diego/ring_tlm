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

#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"

#include "tlm.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"


class Router : public sc_core::sc_module
{
    public:
        // *********************************************
        // Constructor and Methods
        // *********************************************
        SC_HAS_PROCESS(Router);
        Router(sc_core::sc_module_name module_name, unsigned int id);

        virtual tlm::tlm_sync_enum nb_transport_fw(             int id,
                                                                tlm::tlm_generic_payload& trans,
                                                                tlm::tlm_phase& phase, sc_time& delay);

        virtual tlm::tlm_sync_enum nb_transport_bw(             int id,
                                                                tlm::tlm_generic_payload& trans,
                                                                tlm::tlm_phase& phase, sc_time& delay);
        // *********************************************
        // Class attributes
        // *********************************************
        tlm_utils::multi_passthrough_target_socket<Router>      target_socket;
        tlm_utils::multi_passthrough_initiator_socket<Router>   init_socket;

    private:
        // *********************************************
        // Class attributes
        // *********************************************
        int                                                     router_id;

        std::map <tlm::tlm_generic_payload*, unsigned int>      m_id_map;


};

#endif

