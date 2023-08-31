add_subdirectory(adapter)

add_library(daisi_minhton_application STATIC)
target_sources(daisi_minhton_application
        PRIVATE
        minhton_application.cpp
        minhton_application.h
        )
target_link_libraries(daisi_minhton_application
        PUBLIC
            minhton_core_node_sim
            daisi_minhton_logger_ns3
        PRIVATE
            daisi_logger_manager
            daisi_solanet_message_ns3
            daisi_utils
        )

add_library(daisi_minhton_logger_ns3 STATIC)
target_sources(daisi_minhton_logger_ns3
        PRIVATE
        minhton_logger_ns3.cpp
        minhton_logger_ns3.h
        )
target_link_libraries(daisi_minhton_logger_ns3
        PUBLIC
            daisi_logging_definitions
            minhton_core_node_info_sim
            minhton_message_sim
            minhton_logging
            solanet_uuid
        PRIVATE
            ns3::libcore
        )

add_library(daisi_minhton_manager STATIC)
target_sources(daisi_minhton_manager
        PRIVATE
        minhton_manager.cpp
        minhton_manager.h
        minhton_manager_scheduler.cpp
        minhton_manager_scheduler.h
        minhton_manager_scheduler_helper.cpp
        )
target_link_libraries(daisi_minhton_manager
        PUBLIC
            daisi_minhton_application
            daisi_manager
            daisi_manager_scenariofileparser
            daisi_minhton_logger_ns3
            minhton_algorithms_sim
            daisi_minhton_scenariofile
        PRIVATE
            daisi_utils
            minhton_exception_algorithm
            daisi_random_engine
)
target_include_directories(daisi_minhton_manager
        PUBLIC
            ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_minhton_scenariofile STATIC)
target_sources(daisi_minhton_scenariofile
        PRIVATE
            minhton_scenariofile.cpp
            minhton_scenariofile.h
        )
target_link_libraries(daisi_minhton_scenariofile
        PUBLIC
            daisi_manager_general_scenariofile
            daisi_minhton_scenario_steps
            minhton_utils_timeout_lengths_container
            minhton_utils_algorithm_types_container
            daisi_minhton_peer_discovery
        PRIVATE
            daisi_utils
)
target_include_directories(daisi_minhton_scenariofile
        PUBLIC
            ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_minhton_scenario_steps INTERFACE)
target_sources(daisi_minhton_scenario_steps INTERFACE scenario_steps.h)
target_link_libraries(daisi_minhton_scenario_steps
        INTERFACE
            daisi_manager_scenariofile_component
)
target_include_directories(daisi_minhton_scenario_steps
        INTERFACE
            ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_minhton_peer_discovery STATIC)
target_sources(daisi_minhton_peer_discovery
    PRIVATE
        peer_discovery_general.h
        peer_discovery_general.cpp
        peer_discovery_attributes.h
        peer_discovery_attributes.cpp
        peer_discovery_requests.h
        peer_discovery_requests.cpp
)
target_link_libraries(daisi_minhton_peer_discovery
        PUBLIC
            daisi_manager_scenariofile_component
        PRIVATE
            daisi_utils
)
target_include_directories(daisi_minhton_peer_discovery
        INTERFACE
            ${PROJECT_SOURCE_DIR}/src
)
