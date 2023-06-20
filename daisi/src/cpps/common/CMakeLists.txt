add_library(daisi_cpps_common_agv_description INTERFACE)
target_sources(daisi_cpps_common_agv_description
        INTERFACE
        agv_description.h
)
target_link_libraries(daisi_cpps_common_agv_description
        INTERFACE
        ns3::libcore
        daisi_cpps_amr_physical_amr_mobility_model_ns3
        daisi_cpps_model_kinematics
        daisi_cpps_amr_model_amr_static_ability
)

add_library(daisi_cpps_common_boundaries INTERFACE)
target_sources(daisi_cpps_common_boundaries
        INTERFACE
        boundaries.h
)
target_link_libraries(daisi_cpps_common_boundaries
        INTERFACE
        ns3::libcore
)
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
        minhton_logging
        daisi_cpps_agv_agv_logical
        daisi_cpps_to_transport_order
        daisi_cpps_common_cpps_logger_ns3
)

add_library(daisi_cpps_common_cpps_logger_ns3 STATIC)
target_sources(daisi_cpps_common_cpps_logger_ns3
        PRIVATE
        cpps_logger_ns3.cpp
        cpps_logger_ns3.h
)
target_link_libraries(daisi_cpps_common_cpps_logger_ns3
        PUBLIC
        daisi_cpps_model_task
        sola_service
)

add_library(daisi_cpps_common_cpps_manager STATIC)
target_sources(daisi_cpps_common_cpps_manager
        PRIVATE
        cpps_manager.cpp
        cpps_manager.h
)
target_link_libraries(daisi_cpps_common_cpps_manager
        PUBLIC
        ns3::libbridge
        ns3::libcsma
        ns3::libinternet
        ns3::libcore
        daisi_manager
        daisi_cpps_common_cpps_logger_ns3
        daisi_cpps_common_cpps_application
        daisi_cpps_model_kinematics
        daisi_cpps_common_agv_description
        daisi_cpps_negotiation_mrta_configuration
        daisi_cpps_amr_physical_amr_mobility_model_ns3
        daisi_cpps_common_boundaries
        PRIVATE
        daisi_cpps_amr_physical_amr_physical_asset
        daisi_cpps_agv_agv_logical
        ns3::libwifi
        daisi_minhton_logger_ns3
        daisi_natter_logger_ns3
        ns3::libmobility
        daisi_utils
        daisi_random_engine
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
