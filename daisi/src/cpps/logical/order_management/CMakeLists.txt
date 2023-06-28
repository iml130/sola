add_library(daisi_cpps_logical_order_management_order_management_helper STATIC)
target_sources(daisi_cpps_logical_order_management_order_management_helper
    PUBLIC
    order_management_helper.h
    order_management_helper.cpp
)
target_link_libraries(daisi_cpps_logical_order_management_order_management_helper
    daisi_material_flow_model_task
)

add_library(daisi_cpps_logical_order_management_order_management INTERFACE)
target_sources(daisi_cpps_logical_order_management_order_management
    INTERFACE
    order_management.h
)
target_link_libraries(daisi_cpps_logical_order_management_order_management
    INTERFACE
    daisi_material_flow_model_task
    daisi_cpps_amr_amr_description
    daisi_cpps_logical_order_management_order_management_helper
)

add_library(daisi_cpps_logical_order_management_metrics_composition STATIC)
target_sources(daisi_cpps_logical_order_management_metrics_composition
    PUBLIC
    metrics.h
    metrics.cpp
    metrics_composition.h
)
target_link_libraries(daisi_cpps_logical_order_management_metrics_composition
    PUBLIC
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_logical_order_management_auction_based_order_management INTERFACE)
target_sources(daisi_cpps_logical_order_management_auction_based_order_management
    INTERFACE
    order_management.h
)
target_link_libraries(daisi_cpps_logical_order_management_auction_based_order_management
    INTERFACE
    daisi_cpps_logical_order_management_order_management
    daisi_cpps_logical_order_management_metrics_composition
)

add_library(daisi_cpps_logical_order_management_stn_order_management STATIC)
target_sources(daisi_cpps_logical_order_management_stn_order_management
    PUBLIC
    stn_order_management.h
    stn_order_management.cpp
    stn_order_management_components.h
    stn_order_management_components.cpp
)
target_link_libraries(daisi_cpps_logical_order_management_stn_order_management
    PUBLIC
    daisi_cpps_logical_order_management_auction_based_order_management
    daisi_datastructure_simple_temporal_network
    daisi_cpps_amr_amr_mobility_helper
)
add_library(daisi_cpps_logical_order_management_simple_order_management STATIC)
target_sources(daisi_cpps_logical_order_management_simple_order_management
    PUBLIC
    simple_order_management.h
    simple_order_management.cpp
)
target_link_libraries(daisi_cpps_logical_order_management_simple_order_management
    PUBLIC
    daisi_cpps_logical_order_management_order_management
    daisi_cpps_logical_order_management_metrics_composition
    daisi_cpps_amr_amr_mobility_helper
    ns3::libcore
    solanet_serialize
)
target_include_directories(daisi_cpps_logical_order_management_simple_order_management
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)
