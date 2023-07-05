add_subdirectory(scenariofile)

add_library(daisi_cpps_common_cpps_application STATIC)
target_sources(daisi_cpps_common_cpps_application
        PRIVATE
        cpps_application.cpp
        cpps_application.h
)
target_link_libraries(daisi_cpps_common_cpps_application
        PUBLIC
        ns3::libapplications
        ns3::libnetwork
        daisi_cpps_amr_physical_amr_physical_asset
        daisi_cpps_logical_amr_amr_logical_agent
        daisi_cpps_common_cpps_logger_ns3
        daisi_cpps_logical_material_flow_material_flow_logical_agent
)

add_library(daisi_cpps_common_cpps_logger_ns3 STATIC)
target_sources(daisi_cpps_common_cpps_logger_ns3
        PRIVATE
        cpps_logger_ns3_material_flow.cpp
        cpps_logger_ns3_amr.cpp
        cpps_logger_ns3.cpp
        cpps_logger_ns3.h
)
target_link_libraries(daisi_cpps_common_cpps_logger_ns3
        PUBLIC
        sola_service
        daisi_logging_definitions
        daisi_material_flow_model_task

        PRIVATE
        solanet_serializer
        solanet_serialize
        daisi_structure_helpers
        ns3::libcore
)

add_library(daisi_cpps_common_cpps_manager STATIC)
target_sources(daisi_cpps_common_cpps_manager
        PRIVATE
        cpps_manager.cpp
        cpps_manager.h
)
target_link_libraries(daisi_cpps_common_cpps_manager
        PRIVATE
        daisi_manager
        daisi_cpps_common_cpps_logger_ns3
        daisi_cpps_common_cpps_application
        daisi_cpps_amr_physical_amr_mobility_model_ns3
        daisi_cpps_amr_physical_amr_physical_asset
        daisi_cpps_logical_amr_amr_logical_agent
        daisi_cpps_logical_material_flow_material_flow_logical_agent
        daisi_cpps_logical_algorithms_algorithm_config
        solanet_serializer
        daisi_structure_helpers
        ns3::libcore
        ns3::libwifi
        ns3::libmobility
        ns3::libbridge
        ns3::libcsma
        ns3::libinternet
        daisi_utils
        daisi_random_engine
        daisi_cpps_common_scenariofile_cpps_scenariofile
)

if(DAISI_DISABLE_NETWORK_SIMULATION)
        target_compile_definitions(daisi_cpps_common_cpps_manager PRIVATE DAISI_SOLANET_NS3_DISABLE_NETWORKING)
endif()

add_library(daisi_cpps_common_uuid_generator STATIC)
target_sources(daisi_cpps_common_uuid_generator
        PRIVATE
        uuid_generator.cpp
        uuid_generator.h
)
target_include_directories(daisi_cpps_common_uuid_generator
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)
