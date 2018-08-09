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

#include "constants.h"
#include "router.h"

// *********************************************   
// TLM2 backward path non-blocking transport method   
// *********************************************   

Router::Router(sc_core::sc_module_name module_name, unsigned int id) : init_socket("Initiator"), target_socket("Target")
{
    router_id = id;

    // Register callbacks for incoming interface method calls
    target_socket.register_nb_transport_fw(this, &Router::nb_transport_fw);
    init_socket.register_nb_transport_bw(this, &Router::nb_transport_bw);
}

tlm::tlm_sync_enum Router::nb_transport_fw(int id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
    assert (id < target_socket.size());

    // Forward path
    m_id_map[&trans] = id;

    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( address, masked_address);

    std::cout << "---------> Outgoing msg from Router: " << router_id
              << ", Target_Socket: " << target_socket.name()
              << ", Id: " << id
              << ", Init_Socket: " << init_socket.name()
              << ", Phase: " << phase
              << ", Time: " << sc_time_stamp() << "\n";

    tlm::tlm_sync_enum status;

    trans.set_address( masked_address );

    if (router_id == TOP_ROUTER) {

        //std::cout << "Sending message to Memory!\n";
        status = init_socket[1]->nb_transport_fw(trans, phase, delay);

    } else {

        //std::cout << "Sending message to next CPU!\n";
        status = init_socket[0]->nb_transport_fw(trans, phase, delay);
    }
    
    if (status == tlm::TLM_COMPLETED) {
        // Put back original address
        trans.set_address(address);
    }

    return status;
}

tlm::tlm_sync_enum Router::nb_transport_bw(int id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
    assert (id < init_socket.size());

    // Replace original address
    sc_dt::uint64 address = trans.get_address();
    //trans.set_address(compose_address(decode_destination(address), decode_source(address) , address));
    trans.set_address(compose_address(id, address));

    std::cout << "<--------- Incoming msg from Router: " << router_id
              << ", Init_Socket: " << init_socket.name()
              << ", Id: " << id
              << ", Target_Socket: " << target_socket.name()
              << ", Phase: " << phase
              << ", Time: " << sc_time_stamp() << "\n";

    return target_socket[ m_id_map[ &trans ] ]->nb_transport_bw(trans, phase, delay);
}
