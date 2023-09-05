add_library(daisi_manager_scenariofileparser STATIC)

target_sources(daisi_manager_scenariofileparser
    PRIVATE
    scenariofileparser.cpp
    scenariofileparser.h
)

target_link_libraries(daisi_manager_scenariofileparser
    PUBLIC
    yaml-cpp
)

add_library(daisi_manager STATIC)
target_sources(daisi_manager
    PRIVATE
    manager.h
    manager.cpp
)
target_link_libraries(daisi_manager
    PUBLIC
        daisi_manager_general_scenariofile
    PRIVATE
        daisi_logger_manager
        daisi_random_engine
        ns3::libcore
        daisi_utils
)
target_include_directories(daisi_manager
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_manager_old INTERFACE)
target_sources(daisi_manager_old
    INTERFACE
    sola_constants.h
    sola_helper.h
    manager_old.h
)
target_link_libraries(daisi_manager_old
    INTERFACE
    daisi_manager_scenariofileparser
    daisi_logger_manager
    ns3::libnetwork
    ns3::libbridge
    ns3::libcsma
    ns3::libinternet
    daisi_random_engine
    daisi_socket_manager
    daisi_manager_sola_helper
)
target_include_directories(daisi_manager_old
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_manager_scenariofile_component INTERFACE)
target_sources(daisi_manager_scenariofile_component
    INTERFACE
    scenariofile_component.h
)
target_link_libraries(daisi_manager_scenariofile_component
    INTERFACE
    yaml-cpp
    ns3::libcore
)

add_library(daisi_manager_general_scenariofile INTERFACE)
target_sources(daisi_manager_general_scenariofile
    INTERFACE
    general_scenariofile.h
)
target_link_libraries(daisi_manager_general_scenariofile
    INTERFACE
    daisi_manager_scenariofile_component
    yaml-cpp
    ns3::libcore
)

add_library(daisi_manager_core_network)
target_sources(daisi_manager_core_network
    PRIVATE
    core_network.h
    core_network.cpp
)
target_link_libraries(daisi_manager_core_network
    PUBLIC
        ns3::libnetwork
    PRIVATE
        ns3::libinternet
        ns3::libcsma
        ns3::libcore
        ns3::libbridge
        ns3::libmobility
        ns3::libwifi
        daisi_utils
        daisi_manager_sola_constants
)

add_library(daisi_manager_sola_constants INTERFACE sola_constants.h)
target_include_directories(daisi_manager_sola_constants
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_manager_sola_helper INTERFACE)
target_sources(daisi_manager_sola_helper
    INTERFACE
    sola_helper.h
)
target_link_libraries(daisi_manager_sola_helper
    INTERFACE
        ns3::libcore
        daisi_socket_manager
        daisi_utils
)
