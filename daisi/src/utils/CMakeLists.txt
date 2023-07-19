add_library(daisi_utils STATIC)

target_sources(daisi_utils
        PRIVATE
        sola_utils.cpp
        sola_utils.h
        daisi_check.h
        distribution.h
)

target_link_libraries(daisi_utils
        PUBLIC
        ns3::libnetwork
        ns3::libinternet
)

target_include_directories(daisi_utils
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_random_engine INTERFACE random_engine.h)
target_include_directories(daisi_random_engine
        INTERFACE
        ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_structure_helpers STATIC)
set_target_properties(daisi_structure_helpers PROPERTIES LINKER_LANGUAGE CXX)
target_sources(daisi_structure_helpers
        PRIVATE
        structure_helpers.h
)
target_link_libraries(daisi_structure_helpers
        PUBLIC
        solanet_serializer
        ns3::libcore
)
target_include_directories(daisi_structure_helpers
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_socket_manager STATIC)
target_sources(daisi_socket_manager
        PRIVATE
        socket_manager.cpp
        socket_manager.h
)
target_link_libraries(daisi_socket_manager
        PUBLIC
        ns3::libcore
)
target_include_directories(daisi_socket_manager
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)
