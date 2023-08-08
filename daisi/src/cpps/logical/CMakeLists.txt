add_subdirectory(algorithms)
add_subdirectory(amr)
add_subdirectory(material_flow)
add_subdirectory(message)
add_subdirectory(order_management)

add_library(daisi_cpps_logical_logical_agent STATIC)
target_sources(daisi_cpps_logical_logical_agent
    PRIVATE
    logical_agent.cpp
    logical_agent.h
)
target_link_libraries(daisi_cpps_logical_logical_agent
    PUBLIC
    daisi_cpps_logical_algorithms_algorithm_interface
    daisi_sola_sola_ns3_wrapper
    daisi_cpps_logical_message_serializer
    daisi_cpps_common_cpps_logger_ns3
    PRIVATE
    daisi_utils
    daisi_socket_manager
    solanet_uuid
    solanet_uuid_generator_sim
    sola_config_helper_ns3
)
