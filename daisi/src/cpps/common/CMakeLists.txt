add_subdirectory(scenariofile)

add_library(daisi_cpps_common_material_flow_logical_agent_application STATIC)
target_sources(daisi_cpps_common_material_flow_logical_agent_application
        PRIVATE
        material_flow_logical_agent_application.h
        material_flow_logical_agent_application.cpp
)
target_link_libraries(daisi_cpps_common_material_flow_logical_agent_application
        PUBLIC
                ns3::libapplications
                daisi_cpps_logical_material_flow_material_flow_logical_agent
)

add_library(daisi_cpps_common_amr_logical_agent_application STATIC)
target_sources(daisi_cpps_common_amr_logical_agent_application
        PRIVATE
        amr_logical_agent_application.h
        amr_logical_agent_application.cpp
)
target_link_libraries(daisi_cpps_common_amr_logical_agent_application
        PUBLIC
                ns3::libapplications
                daisi_cpps_logical_amr_amr_logical_agent
)

add_library(daisi_cpps_common_amr_physical_asset_application STATIC)
target_sources(daisi_cpps_common_amr_physical_asset_application
        PRIVATE
        amr_physical_asset_application.h
        amr_physical_asset_application.cpp
)
target_link_libraries(daisi_cpps_common_amr_physical_asset_application
        PUBLIC
                ns3::libapplications
                daisi_cpps_amr_physical_amr_physical_asset
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
        solanet_uuid

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
        PUBLIC
              daisi_cpps_amr_amr_topology
              daisi_manager_core_network
              daisi_manager
              ns3::libnetwork
              daisi_cpps_common_scenariofile_cpps_scenariofile
        PRIVATE
                daisi_cpps_amr_model_amr_fleet
                daisi_cpps_amr_physical_amr_mobility_model_ns3
                daisi_cpps_amr_physical_amr_physical_asset
                daisi_cpps_amr_model_amr_static_ability
                daisi_cpps_logical_amr_amr_logical_agent
                daisi_cpps_common_material_flow_logical_agent_application
                daisi_cpps_common_amr_logical_agent_application
                daisi_cpps_common_amr_physical_asset_application
                daisi_cpps_logical_material_flow_material_flow_logical_agent
                daisi_manager_sola_helper
                ns3::libcore
                ns3::libmobility
                ns3::libinternet
                daisi_utils
                daisi_random_engine
                daisi_logger_manager
)

if(DAISI_DISABLE_NETWORK_SIMULATION)
        target_compile_definitions(daisi_cpps_common_cpps_manager PRIVATE DAISI_SOLANET_NS3_DISABLE_NETWORKING)
endif()

add_library(daisi_cpps_common_cpps_communicator INTERFACE cpps_communicator.h)
target_link_libraries(daisi_cpps_common_cpps_communicator
        INTERFACE
        ManagementOverlayMINHTONSim
        EventDisseminationMinhcastSim
        SOLA_ns3_interface
        NetworkUDPSim
)
