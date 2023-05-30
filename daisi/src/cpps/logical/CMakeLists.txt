
add_subdirectory(order_management)
add_subdirectory(algorithms)
add_subdirectory(amr)
add_subdirectory(material_flow)
add_subdirectory(message)

add_library(daisi_cpps_logical_logical_agent STATIC)
target_sources(daisi_cpps_logical_logical_agent
    PUBLIC
    logical_agent.cpp
    logical_agent.h
)
target_link_libraries(daisi_cpps_logical_logical_agent
    PUBLIC
    daisi_cpps_logical_algorithms_algorithm_interface
    daisi_sola_sola_ns3_wrapper
    daisi_cpps_logical_message_serializer
    daisi_cpps_common_uuid_generator
)
target_include_directories(daisi_cpps_logical_logical_agent
    PUBLIC
    ${DAISI_SOURCE_DIR}/src)