add_library(daisi_cpps_amr_pysical_amr_asset_connector STATIC)
target_sources(daisi_cpps_amr_pysical_amr_asset_connector
    PRIVATE
    amr_asset_connector.h
    amr_asset_connector_ns3.cpp
)
target_link_libraries(daisi_cpps_amr_pysical_amr_asset_connector
    PUBLIC
    daisi_cpps_amr_amr_description
    daisi_cpps_amr_amr_mobility_helper
    daisi_cpps_amr_amr_topology
    daisi_cpps_amr_physical_functionality
    PRIVATE
    ns3::libcore
    daisi_cpps_amr_physical_amr_mobility_model_ns3
    daisi_cpps_amr_physical_amr_mobility_status
    daisi_cpps_amr_physical_functionality
    daisi_structure_helpers
)
target_include_directories(daisi_cpps_amr_pysical_amr_asset_connector
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_cpps_amr_physical_amr_mobility_model_ns3 STATIC)
target_sources(daisi_cpps_amr_physical_amr_mobility_model_ns3
    PRIVATE
    amr_mobility_model_ns3.h
    amr_mobility_model_ns3.cpp
)
target_link_libraries(daisi_cpps_amr_physical_amr_mobility_model_ns3
    PUBLIC
    daisi_cpps_amr_amr_description
    daisi_cpps_amr_amr_mobility_helper
    daisi_cpps_amr_amr_topology
    daisi_cpps_amr_physical_functionality
    PRIVATE
    ns3::libcore
    ns3::libmobility
    daisi_cpps_amr_physical_amr_mobility_status
    daisi_cpps_amr_physical_functionality
    daisi_structure_helpers
)
target_include_directories(daisi_cpps_amr_physical_amr_mobility_model_ns3
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_cpps_amr_physical_amr_mobility_status INTERFACE)
target_sources(daisi_cpps_amr_physical_amr_mobility_status
    INTERFACE
    amr_mobility_status.h
)
target_link_libraries(daisi_cpps_amr_physical_amr_mobility_status
    INTERFACE
    daisi_structure_helpers
)

add_library(daisi_cpps_amr_physical_amr_physical_asset STATIC)
target_sources(daisi_cpps_amr_physical_amr_physical_asset
    PRIVATE
    amr_physical_asset.h
    amr_physical_asset.cpp
)
target_link_libraries(daisi_cpps_amr_physical_amr_physical_asset
    PUBLIC
    daisi_cpps_amr_pysical_amr_asset_connector
    daisi_cpps_amr_message_amr_order_info
    ns3::libcore
    PRIVATE
    daisi_cpps_amr_message_amr_state
    daisi_cpps_amr_message_serializer
    daisi_cpps_packet
)
target_include_directories(daisi_cpps_amr_physical_amr_physical_asset
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_cpps_amr_physical_amr_order INTERFACE)
target_sources(daisi_cpps_amr_physical_amr_order
    INTERFACE
    amr_order.h
)
target_link_libraries(daisi_cpps_amr_physical_amr_order
    INTERFACE
    daisi_cpps_model_ability
    daisi_cpps_amr_physical_functionality
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_amr_physical_functionality INTERFACE)
target_sources(daisi_cpps_amr_physical_functionality
    INTERFACE
    functionality.h
)
target_link_libraries(daisi_cpps_amr_physical_functionality
    INTERFACE
    daisi_cpps_amr_physical_load
    daisi_cpps_amr_physical_move_to
    daisi_cpps_amr_physical_navigate
    daisi_cpps_amr_unload
)

add_library(daisi_cpps_amr_physical_load INTERFACE)
target_sources(daisi_cpps_amr_physical_load
    INTERFACE
    load.h
)
target_link_libraries(daisi_cpps_amr_physical_load
    INTERFACE
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_amr_physical_move_to INTERFACE)
target_sources(daisi_cpps_amr_physical_move_to
    INTERFACE
    move_to.h
)
target_link_libraries(daisi_cpps_amr_physical_move_to
    INTERFACE
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_amr_physical_navigate INTERFACE)
target_sources(daisi_cpps_amr_physical_navigate
    INTERFACE
    navigate.h
)
target_link_libraries(daisi_cpps_amr_physical_navigate
    INTERFACE
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_amr_unload INTERFACE)
target_sources(daisi_cpps_amr_unload
    INTERFACE
    unload.h
)
target_link_libraries(daisi_cpps_amr_unload
    INTERFACE
    daisi_structure_helpers
    solanet_serialize
)

add_library(daisi_cpps_amr_physical_material_flow_functionality_mapping STATIC)
target_sources(daisi_cpps_amr_physical_material_flow_functionality_mapping
    PUBLIC
    material_flow_functionality_mapping.cpp
    material_flow_functionality_mapping.h
)
target_link_libraries(daisi_cpps_amr_physical_material_flow_functionality_mapping
    PUBLIC
    daisi_cpps_amr_amr_mobility_helper
    daisi_cpps_amr_physical_functionality
    daisi_material_flow_model_task
)
