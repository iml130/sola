include(create_ns3_executable)

add_library(SOLA_ns3_interface INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/sola.h)
target_include_directories(SOLA_ns3_interface INTERFACE ${SOLA_SOURCE_DIR}/include)
target_link_libraries(SOLA_ns3_interface INTERFACE
        sola_service
        sola_logger_interface
        solanet_uuid
        solanet_uuid_generator_sim
)

add_library(daisi_sola_sola_wrapper INTERFACE sola_wrapper.h)
target_link_libraries(daisi_sola_sola_wrapper
        INTERFACE
        SOLA_ns3_interface
        ManagementOverlayMINHTONSim
        EventDisseminationMinhcastSim
)

add_library(daisi_sola_application STATIC)
target_sources(daisi_sola_application
        PRIVATE
        sola_application.h
        sola_application.cpp
)
target_link_libraries(daisi_sola_application
        PUBLIC
        ns3::libcore
        daisi_sola_logger_ns3
        daisi_sola_sola_wrapper
        PRIVATE
        daisi_utils
        minhton_utils_config_reader
        sola_config_helper_ns3
        solanet_uuid
        daisi_logger_manager
)

add_library(daisi_sola_logger_ns3 STATIC)
target_sources(daisi_sola_logger_ns3
        PRIVATE
        sola_logger_ns3.h
        sola_logger_ns3.cpp
)
target_link_libraries(daisi_sola_logger_ns3
        PUBLIC
        daisi_logging_sqlite_helper
        daisi_cpps_common_cpps_logger_ns3
        sola_logger_interface
        PRIVATE
        ns3::libcore
        solanet_serializer
)

add_library(daisi_sola_manager STATIC)
target_sources(daisi_sola_manager
        PRIVATE
        sola_manager.cpp
        sola_manager.h
        sola_manager_scheduler.cpp
)
target_link_libraries(daisi_sola_manager
        PUBLIC
                daisi_manager
                daisi_sola_application
                daisi_sola_logger_ns3
                daisi_manager_core_network
                daisi_sola_scenariofile
                ns3::libcore
        PRIVATE
                daisi_manager_sola_helper
)
target_include_directories(daisi_sola_manager
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

# Rebuild SOLA for ns-3 simulation

# EventDisseminationMinhcast
add_library(EventDisseminationMinhcastSim STATIC)
target_sources(EventDisseminationMinhcastSim
        PRIVATE
        ${SOLA_SOURCE_DIR}/src/event_dissemination/natter/event_dissemination_minhcast.cpp
        ${SOLA_SOURCE_DIR}/include/SOLA/event_dissemination_minhcast.h event_dissemination_minhcast_sim.cpp
)
target_link_libraries(EventDisseminationMinhcastSim
        PUBLIC
        SOLAMessage
        EventDissemination
        natter_minhcast_sim
        minhton_core_node_sim
        Storage
        PRIVATE
        sola_check
        solanet_serializer
        ns3::libcore
)
target_include_directories(EventDisseminationMinhcastSim PUBLIC ${SOLA_SOURCE_DIR}/include)

# Management Overlay MINHTON
add_library(ManagementOverlayMINHTONSim STATIC $<TARGET_OBJECTS:ManagementOverlayMINHTONObj>)
target_link_libraries(ManagementOverlayMINHTONSim
        PUBLIC
        Storage
        minhton_core_node_sim
        PRIVATE
        minhton_core_definitions
        minhton_utils_config_reader
)
target_include_directories(ManagementOverlayMINHTONSim
        PUBLIC
        ${SOLA_SOURCE_DIR}/include)

add_library(sola_config_helper_ns3 INTERFACE config_helper_ns3.h)
target_link_libraries(sola_config_helper_ns3 INTERFACE
        ManagementOverlayMINHTONSim
        EventDisseminationMinhcastSim
        daisi_logger_manager
)

add_library(daisi_sola_management_minhton_helper INTERFACE management_minhton_helper.h)
target_link_libraries(daisi_sola_management_minhton_helper INTERFACE ManagementOverlayMINHTONSim)

add_library(daisi_sola_scenariofile STATIC)
target_sources(daisi_sola_scenariofile
        PRIVATE
        sola_scenariofile.cpp
        sola_scenariofile.h
)
target_link_libraries(daisi_sola_scenariofile
        PUBLIC
                daisi_manager_general_scenariofile
                ns3::libcore
        PRIVATE
                daisi_utils
)
target_include_directories(daisi_sola_scenariofile
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

create_ns3_executable(
        daisi::sola_ns3::SolaManager # Class
        sola-ns3/sola_manager.h # Include
        daisi_sola_manager # CMake target
        SOLA # Name
)
