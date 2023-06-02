add_library(PathPlanning STATIC)
target_sources(PathPlanning
    PRIVATE
        path_planning_application.cpp
        path_planning_application.h
        path_planning_manager.cpp
        path_planning_manager.h
        agv_logical.cpp
        agv_logical.h
        station.h
        agv_info.h
        pickup_station.cpp
        pickup_station.h
        agv_queue.h
        agv_queue.cpp
        delivery_station.cpp
        delivery_station.h
        connection_points.h
        destination.h
        route.h
        time_calculation_helper.cpp
        next_to_mode.h
        time_calculation_helper.h
        message/serializer.cpp
        path_planning_logger_ns_3.cpp
        path_planning_logger_ns_3.h

        # Consensus
        consensus/consensus_base.h
        consensus/consensus.cpp
        consensus/route_calculation_helper.cpp
        consensus/route_calculation_helper.h

        ## Paxos
        consensus/paxos/paxos_acceptor.cpp
        consensus/paxos/paxos_acceptor.h
        consensus/paxos/paxos_consensus.cpp
        consensus/paxos/paxos_consensus.h
        consensus/paxos/paxos_data.h
        consensus/paxos/paxos_proposer.cpp
        consensus/paxos/paxos_proposer.h
        consensus/paxos/paxos_replication_manager.cpp
        consensus/paxos/constants.h

        ## Central
        consensus/central/central_participant.cpp
        consensus/central/central_server.cpp
)

target_link_libraries(PathPlanning
    PUBLIC
        ns3::libapplications
        ns3::libnetwork
        minhton_logging
        daisi_manager
        daisi_cpps_agv_agv_physical_basic
        daisi_cpps_agv_agv_logical
        daisi_cpps_to_transport_order
        daisi_cpps_common_cpps_logger_ns3
        daisi_cpps_model_task
        daisi_sola_sola_ns3_wrapper
        daisi_logging_sqlite_helper
        Cereal
    PRIVATE
        daisi_logger_manager
        daisi_random_engine
        minhton_utils_config_reader
        ns3::libwifi
        ns3::libcsma
)

if(DAISI_DISABLE_NETWORK_SIMULATION)
    target_compile_definitions(PathPlanning PRIVATE DAISI_SOLANET_NS3_DISABLE_NETWORKING)
endif()
