add_library(daisi_cpps_logical_amr_amr_logical_agent STATIC)
target_sources(daisi_cpps_logical_amr_amr_logical_agent PRIVATE
    amr_logical_agent.cpp
    amr_logical_agent.h
    amr_logical_execution_state.cpp
    amr_logical_execution_state.h
)
target_link_libraries(daisi_cpps_logical_amr_amr_logical_agent
    PUBLIC
    daisi_cpps_logical_logical_agent
    daisi_cpps_logical_algorithms_assignment_iterated_auction_assignment_participant
    daisi_cpps_logical_algorithms_assignment_centralized_participant
    daisi_cpps_amr_message_serializer
    ns3::libcore
    daisi_cpps_amr_physical_material_flow_functionality_mapping
    daisi_cpps_logical_order_management_order_management
    daisi_cpps_logical_order_management_stn_order_management
    daisi_cpps_logical_order_management_simple_order_management
    daisi_utils
    NetworkUDPMessage
    PRIVATE
    solanet_uuid
    solanet_uuid_generator_sim
    daisi_cpps_logical_message_material_flow_update
    daisi_logger_manager
)

target_include_directories(daisi_cpps_amr_amr_description INTERFACE
    ${DAISI_SOURCE_DIR}/src
)
