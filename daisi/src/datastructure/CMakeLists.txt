add_library(daisi_datastructure_directed_graph INTERFACE)
target_sources(daisi_datastructure_directed_graph
    INTERFACE
    directed_graph.h
    directed_graph_impl.h
)
target_include_directories(daisi_datastructure_directed_graph 
    INTERFACE 
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_datastructure_weighted_directed_graph INTERFACE)
target_sources(daisi_datastructure_weighted_directed_graph
    INTERFACE
    weighted_directed_graph.h
    weighted_directed_graph_impl.h
)
target_include_directories(daisi_datastructure_weighted_directed_graph 
    INTERFACE 
    daisi_datastructure_directed_graph
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_datastructure_simple_temporal_network INTERFACE)
target_sources(daisi_datastructure_simple_temporal_network
    INTERFACE
    simple_temporal_network.h
    simple_temporal_network_impl.h
)
target_include_directories(daisi_datastructure_simple_temporal_network 
    INTERFACE 
    daisi_datastructure_weighted_directed_graph
    ${DAISI_SOURCE_DIR}/src
)
