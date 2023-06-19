add_subdirectory(message)
add_subdirectory(physical)
add_subdirectory(model)

add_library(daisi_cpps_amr_amr_description INTERFACE)
target_sources(daisi_cpps_amr_amr_description
    INTERFACE
    amr_description.h
)
target_link_libraries(daisi_cpps_amr_amr_description INTERFACE
    solanet_serialize
    daisi_cpps_amr_amr_kinematics
    daisi_cpps_amr_amr_load_handling_unit
    daisi_cpps_amr_amr_physical_properties
    daisi_cpps_amr_amr_properties
)

add_library(daisi_cpps_amr_amr_kinematics INTERFACE)
target_sources(daisi_cpps_amr_amr_kinematics
    INTERFACE
    amr_kinematics.h
)
target_link_libraries(daisi_cpps_amr_amr_kinematics
    INTERFACE
    solanet_serialize
)

add_library(daisi_cpps_amr_amr_load_handling_unit INTERFACE)
target_sources(daisi_cpps_amr_amr_load_handling_unit
    INTERFACE
    amr_load_handling_unit.h
)
target_link_libraries(daisi_cpps_amr_amr_load_handling_unit INTERFACE
    solanet_serialize
    daisi_cpps_amr_model_amr_static_ability
)

add_library(daisi_cpps_amr_amr_mobility_helper STATIC)
target_sources(daisi_cpps_amr_amr_mobility_helper
    PRIVATE
    amr_mobility_helper.h
    amr_mobility_helper.cpp
)
target_link_libraries(daisi_cpps_amr_amr_mobility_helper
    PUBLIC
    daisi_cpps_amr_amr_topology
    daisi_cpps_amr_amr_description
    PRIVATE
    daisi_cpps_amr_physical_amr_mobility_status
    daisi_cpps_amr_physical_functionality
    daisi_structure_helpers
)
target_include_directories(daisi_cpps_amr_amr_mobility_helper
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_cpps_amr_amr_physical_properties INTERFACE)
target_sources(daisi_cpps_amr_amr_physical_properties
    INTERFACE
    amr_physical_properties.h
)
target_link_libraries(daisi_cpps_amr_amr_physical_properties INTERFACE
    solanet_serialize
)

add_library(daisi_cpps_amr_amr_properties INTERFACE)
target_sources(daisi_cpps_amr_amr_properties
    INTERFACE
    amr_properties.h
)
target_link_libraries(daisi_cpps_amr_amr_properties
    INTERFACE
    daisi_cpps_amr_model_amr_static_ability
    daisi_cpps_amr_physical_functionality
    solanet_serialize
)

add_library(daisi_cpps_amr_amr_topology STATIC)
target_sources(daisi_cpps_amr_amr_topology
    PRIVATE
    amr_topology.h
    amr_topology.cpp
)
target_link_libraries(daisi_cpps_amr_amr_topology
    PUBLIC
    solanet_serialize
    PRIVATE
    daisi_structure_helpers
)
target_include_directories(daisi_cpps_amr_amr_topology
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)
