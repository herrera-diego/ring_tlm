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

    sc_dt::uint64 address = trans.get_address();
    unsigned int target_nr = decode_dest(address);
    
    std::cout << "---------> Outgoing msg from Router: " << router_id
              << ", Transaction: " << decode_transID(trans.get_address())
              << ", Phase: " << phase
              << ", SRC: " << decode_src(trans.get_address())
              << ", DST: " << decode_dest(trans.get_address())
              << ", Delay: " << delay
              << ", Time: " << sc_time_stamp() << "\n";

    tlm::tlm_sync_enum status;

    if (router_id == target_nr && target_nr != TOP_ROUTER) {

        status = target_socket[0]->nb_transport_bw(trans, phase, delay);
    }

    else if (router_id == target_nr) {

        //std::cout << "Sending message to Destiny!\n";
        status = init_socket[1]->nb_transport_fw(trans, phase, delay);

    } else {

        //std::cout << "Sending message to next Router!\n";
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

    std::cout << "<--------- Incoming msg from Router: " << router_id
              << ", Transaction: " << decode_transID(trans.get_address())
              << ", Phase: " << phase
              << ", SRC: " << decode_src(trans.get_address())
              << ", DST: " << decode_dest(trans.get_address())
              << ", Delay: " << delay
              << ", Time: " << sc_time_stamp() << "\n";

    return init_socket[0]->nb_transport_fw(trans, phase, delay);
}
