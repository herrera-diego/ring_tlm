
#include "constants.h"
#include "router.h"

// *********************************************   
// TLM2 backward path non-blocking transport method   
// *********************************************   

Router::Router(sc_core::sc_module_name module_name, unsigned int id) : init_socket("Initiator"), target_socket("Target"), LATENCY(10, SC_NS)
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
    m_id_map[ &trans ] = id;

    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( address, masked_address);

    std::cout << "Router: " << router_id << " Caller: " << target_socket.name() << " ID: " << id
        << " Socket.size(): " << target_socket.size() << "\n";

    tlm::tlm_sync_enum status;

    trans.set_address( masked_address );

    if (router_id == TOP_ROUTER) {

        std::cout << "Sending message to Memory!\n";
        status = init_socket[1]->nb_transport_fw(trans, phase, delay);

    } else {

        std::cout << "Sending message to next CPU!\n";
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
    int socket;

    assert (id < init_socket.size());

    // Backward path

    // Replace original address
    sc_dt::uint64 address = trans.get_address();
    trans.set_address(compose_address(id, address));

    socket = m_id_map[&trans];
    m_id_map.erase(&trans);

    return target_socket[socket]->nb_transport_bw(trans, phase, delay);
}

// Simple fixed address decoding
// In this example, for clarity, the address is passed through unmodified to the target
inline unsigned int Router::decode_address(sc_dt::uint64 address, sc_dt::uint64& masked_address)
{
    unsigned int target_nr = static_cast<unsigned int>( address & 0x1 );
    masked_address = address;
    return target_nr;
}

inline sc_dt::uint64 Router::compose_address(unsigned int target_nr, sc_dt::uint64 address)
{
    return address;
}
