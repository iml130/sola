add_library(daisi_cpps_common_scenariofile_cpps_scenariofile INTERFACE)
target_sources(daisi_cpps_common_scenariofile_cpps_scenariofile
    INTERFACE
    cpps_scenariofile.h
)
target_link_libraries(daisi_cpps_common_scenariofile_cpps_scenariofile
    INTERFACE
    daisi_manager_general_scenariofile
    daisi_cpps_common_scenariofile_amr_description_scenario
    daisi_cpps_common_scenariofile_material_flow_description_scenario
    daisi_cpps_common_scenariofile_spawn_info_scenario
    yaml-cpp
)

add_library(daisi_cpps_common_scenariofile_amr_description_scenario INTERFACE)
target_sources(daisi_cpps_common_scenariofile_amr_description_scenario
    INTERFACE
    amr_description_scenario.h
)
target_link_libraries(daisi_cpps_common_scenariofile_amr_description_scenario
    INTERFACE
    daisi_manager_scenariofile_component
    daisi_cpps_amr_amr_description
    yaml-cpp
)

add_library(daisi_cpps_common_scenariofile_material_flow_description_scenario INTERFACE)
target_sources(daisi_cpps_common_scenariofile_material_flow_description_scenario
    INTERFACE
    material_flow_scenario.h
)
target_link_libraries(daisi_cpps_common_scenariofile_material_flow_description_scenario
    INTERFACE
    daisi_manager_scenariofile_component
    yaml-cpp
)

add_library(daisi_cpps_common_scenariofile_spawn_info_scenario INTERFACE)
target_sources(daisi_cpps_common_scenariofile_spawn_info_scenario
    INTERFACE
    spawn_info_scenario.h
)
target_link_libraries(daisi_cpps_common_scenariofile_spawn_info_scenario
    INTERFACE
    daisi_manager_scenariofile_component
    yaml-cpp
)
