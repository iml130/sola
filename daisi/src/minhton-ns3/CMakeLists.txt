add_subdirectory(adapter)

add_library(daisi_minhton_application STATIC)
target_sources(daisi_minhton_application
        PRIVATE
        minhton_application.cpp
        minhton_application.h
        )
target_link_libraries(daisi_minhton_application
        PUBLIC
            daisi_minhton_node_ns3
            daisi_logger_manager
            daisi_minhton_logger_ns3
        PRIVATE
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

add_library(daisi_minhton_node_ns3 STATIC)
target_sources(daisi_minhton_node_ns3
        PRIVATE
        minhton_node_ns3.cpp
        minhton_node_ns3.h
        )
target_link_libraries(daisi_minhton_node_ns3
        PUBLIC
            minhton_algorithms_sim
            minhton_core_node_sim
            ns3::libcore
            daisi_solanet_message_ns3
        PRIVATE
            minhton_logging
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
        PRIVATE
            daisi_utils
            minhton_exception_algorithm
            daisi_random_engine
            daisi_minhton_manager_scenariofile_helper
)
target_include_directories(daisi_minhton_manager
        PUBLIC
            ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_minhton_manager_scenariofile_helper STATIC minhton_scenariofile_helper.cpp)
target_link_libraries(daisi_minhton_manager_scenariofile_helper
    PUBLIC
    minhton_utils_algorithm_types_container
    minhton_utils_timeout_lengths_container
)
