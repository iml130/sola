add_library(daisi_cpps_logical_task_management_task_management_helper STATIC)
target_sources(daisi_cpps_logical_task_management_task_management_helper
    PRIVATE
    task_management_helper.h
    task_management_helper.cpp
)
target_link_libraries(daisi_cpps_logical_task_management_task_management_helper
    daisi_material_flow_model_task
)

add_library(daisi_cpps_logical_task_management_task_management INTERFACE)
target_sources(daisi_cpps_logical_task_management_task_management
    INTERFACE
    task_management.h
)
target_link_libraries(daisi_cpps_logical_task_management_task_management
    INTERFACE
    daisi_material_flow_model_task
    daisi_cpps_amr_amr_description
    daisi_cpps_logical_task_management_task_management_helper
)

add_library(daisi_cpps_logical_task_management_metrics_composition STATIC)
target_sources(daisi_cpps_logical_task_management_metrics_composition
    PRIVATE
    metrics.h
    metrics.cpp
    metrics_composition.h
)
target_link_libraries(daisi_cpps_logical_task_management_metrics_composition
    PUBLIC
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_logical_task_management_auction_based_task_management INTERFACE)
target_sources(daisi_cpps_logical_task_management_auction_based_task_management
    INTERFACE
    task_management.h
)
target_link_libraries(daisi_cpps_logical_task_management_auction_based_task_management
    INTERFACE
    daisi_cpps_logical_task_management_task_management
    daisi_cpps_logical_task_management_metrics_composition
)

add_library(daisi_cpps_logical_task_management_stn_task_management STATIC)
target_sources(daisi_cpps_logical_task_management_stn_task_management
    PRIVATE
    stn_task_management.h
    stn_task_management.cpp
    stn_task_management_components.h
    stn_task_management_components.cpp
)
target_link_libraries(daisi_cpps_logical_task_management_stn_task_management
    PUBLIC
    daisi_cpps_logical_task_management_auction_based_task_management
    daisi_datastructure_simple_temporal_network
    daisi_cpps_amr_amr_mobility_helper
)
add_library(daisi_cpps_logical_task_management_simple_task_management STATIC)
target_sources(daisi_cpps_logical_task_management_simple_task_management
    PRIVATE
    simple_task_management.h
    simple_task_management.cpp
)
target_link_libraries(daisi_cpps_logical_task_management_simple_task_management
    PUBLIC
    daisi_cpps_logical_task_management_task_management
    daisi_cpps_logical_task_management_metrics_composition
    daisi_cpps_amr_amr_mobility_helper
    ns3::libcore
    solanet_serialize
)
target_include_directories(daisi_cpps_logical_task_management_simple_task_management
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)
