add_library(daisi_cpps_agv_agv_logical STATIC)
target_sources(daisi_cpps_agv_agv_logical
        PRIVATE
        agv_logical.cpp
        agv_logical.h
)
target_link_libraries(daisi_cpps_agv_agv_logical
        PUBLIC
        ns3::libcore
        ns3::libmobility
        daisi_sola_sola_ns3_wrapper
        daisi_cpps_common_agv_description
        daisi_cpps_amr_physical_amr_mobility_model_ns3
        daisi_cpps_model_kinematics
        daisi_cpps_model_task
        daisi_cpps_agv_topology
        daisi_cpps_common_cpps_logger_ns3
        
        daisi_cpps_task_allocation_participant_task_allocation_participant
        daisi_cpps_negotiation_mrta_configuration
        daisi_cpps_negotiation_task_management_task_management
        daisi_cpps_amr_message_amr_state
        
        PRIVATE
        daisi_cpps_negotiation_task_management_basic_list_task_management
        daisi_cpps_negotiation_task_management_greedy_tsp_list_task_management
        daisi_cpps_negotiation_task_management_stn_task_management
        daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_tepssi
        daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_modified
        
        daisi_cpps_negotiation_utility_utility_dimensions
        daisi_cpps_negotiation_utility_utility_evaluator
        daisi_cpps_negotiation_utility_utility_evaluator
        daisi_cpps_negotiation_utility_accumulated_utility_dimensions
        daisi_cpps_negotiation_utils_directed_graph
        daisi_cpps_negotiation_utils_simple_temporal_network
        daisi_cpps_negotiation_utils_simple_temporal_network_components
        daisi_cpps_negotiation_utils_overload

        daisi_cpps_packet
        daisi_utils
        daisi_cpps_amr_message_serializer
        daisi_cpps_common_uuid_generator

        minhton_logging
        minhton_utils_config_reader
)
target_include_directories(daisi_cpps_agv_agv_logical
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_agv_agv_physical_basic STATIC)
target_sources(daisi_cpps_agv_agv_physical_basic
        PRIVATE
        agv_physical_basic.h
        agv_physical_basic.cpp
        )
target_link_libraries(daisi_cpps_agv_agv_physical_basic
        PUBLIC
        ns3::libcore
        daisi_cpps_model_task
        daisi_cpps_agv_pose
        daisi_cpps_agv_topology
        daisi_cpps_model_kinematics
        daisi_cpps_amr_physical_amr_mobility_model_ns3
        daisi_cpps_agv_agv_states
        daisi_cpps_common_agv_description
        PRIVATE
        daisi_cpps_packet
        )
target_include_directories(daisi_cpps_agv_agv_physical_basic
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)
target_compile_definitions(daisi_cpps_agv_agv_physical_basic PRIVATE NS3_LOG_ENABLE)

add_library(daisi_cpps_agv_agv_states INTERFACE)
target_sources(daisi_cpps_agv_agv_states
        INTERFACE
        agv_states.h
)

add_library(daisi_cpps_agv_pose STATIC)
target_sources(daisi_cpps_agv_pose
        PRIVATE
        pose.cpp
        pose.h
        )
target_link_libraries(daisi_cpps_agv_pose
        PUBLIC
            ns3::libcore
        )
target_include_directories(daisi_cpps_agv_pose
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_agv_topology STATIC)
target_sources(daisi_cpps_agv_topology
        PRIVATE
        topology.cpp
        topology.h
        )
target_link_libraries(daisi_cpps_agv_topology
        PUBLIC
            ns3::libcore
)
target_include_directories(daisi_cpps_agv_topology
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)
