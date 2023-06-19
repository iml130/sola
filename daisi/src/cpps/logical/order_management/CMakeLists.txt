add_library(daisi_cpps_logical_order_management_order_management INTERFACE)
target_sources(daisi_cpps_logical_order_management_order_management
    INTERFACE
    order_management.h
)
target_link_libraries(daisi_cpps_logical_order_management_order_management
    INTERFACE
    daisi_material_flow_model_task
)

add_library(daisi_cpps_logical_order_management_metrics_composition INTERFACE)
target_sources(daisi_cpps_logical_order_management_metrics_composition
    INTERFACE
    metrics.h
    metrics_composition.h
)
target_link_libraries(daisi_cpps_logical_order_management_metrics_composition
    INTERFACE
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
