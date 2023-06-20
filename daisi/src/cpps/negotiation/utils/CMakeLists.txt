add_library(daisi_cpps_negotiation_utils_directed_graph STATIC)
target_sources(daisi_cpps_negotiation_utils_directed_graph
        PRIVATE
        directed_graph.cpp
        directed_graph.h
)
target_link_libraries(daisi_cpps_negotiation_utils_directed_graph
        PUBLIC
        daisi_cpps_negotiation_utils_simple_temporal_network_components
        solanet_serialize
)
target_include_directories(daisi_cpps_negotiation_utils_directed_graph
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utils_simple_temporal_network_components INTERFACE "")
target_sources(daisi_cpps_negotiation_utils_simple_temporal_network_components
        INTERFACE
        simple_temporal_network_components.h
)
target_link_libraries(daisi_cpps_negotiation_utils_simple_temporal_network_components
        INTERFACE
        ns3::libcore
)
target_include_directories(daisi_cpps_negotiation_utils_simple_temporal_network_components
        INTERFACE
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utils_simple_temporal_network STATIC)
target_sources(daisi_cpps_negotiation_utils_simple_temporal_network
        PRIVATE
        simple_temporal_network.cpp
        simple_temporal_network.h
)
target_link_libraries(daisi_cpps_negotiation_utils_simple_temporal_network
        PUBLIC
        daisi_cpps_negotiation_utils_simple_temporal_network_components
        daisi_cpps_negotiation_utils_directed_graph
        daisi_cpps_model_task
        daisi_cpps_model_kinematics
        daisi_cpps_negotiation_task_management_insert_info
        daisi_cpps_negotiation_utility_accumulated_utility_dimensions
)
target_include_directories(daisi_cpps_negotiation_utils_simple_temporal_network
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utils_precedence_graph_components INTERFACE "")
target_sources(daisi_cpps_negotiation_utils_precedence_graph_components
        INTERFACE
        precedence_graph_components.h
)
target_link_libraries(daisi_cpps_negotiation_utils_precedence_graph_components
        INTERFACE
        ns3::libcore
)
target_include_directories(daisi_cpps_negotiation_utils_precedence_graph_components
        INTERFACE
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utils_precedence_graph STATIC)
target_sources(daisi_cpps_negotiation_utils_precedence_graph
        PRIVATE
        precedence_graph.cpp
        precedence_graph.h
)
target_link_libraries(daisi_cpps_negotiation_utils_precedence_graph
        PUBLIC
        daisi_cpps_negotiation_utils_directed_graph
        daisi_cpps_negotiation_utils_precedence_graph_components
        daisi_cpps_model_task
        daisi_cpps_model_agv_fleet
        daisi_cpps_negotiation_utils_simple_temporal_network
)
target_include_directories(daisi_cpps_negotiation_utils_precedence_graph
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utils_overload INTERFACE)
target_sources(daisi_cpps_negotiation_utils_overload
        INTERFACE
        overload.h
)
target_link_libraries(daisi_cpps_negotiation_utils_overload
        INTERFACE
)

add_library(daisi_cpps_negotiation_utils_kmeans INTERFACE)
target_sources(daisi_cpps_negotiation_utils_kmeans
        INTERFACE
        kmeans.h
)
target_link_libraries(daisi_cpps_negotiation_utils_kmeans
        INTERFACE
)
