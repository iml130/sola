add_library(daisi_network_tcp_definitions INTERFACE definitions.h)

add_library(daisi_network_tcp_framing_manager STATIC)
target_sources(daisi_network_tcp_framing_manager
    PRIVATE
        framing_manager.cpp
        framing_manager.h
)
target_link_libraries(daisi_network_tcp_framing_manager
    PRIVATE
        daisi_utils
)
target_include_directories(daisi_network_tcp_framing_manager
    PUBLIC
        ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_network_tcp_client STATIC)
target_sources(daisi_network_tcp_client
    PRIVATE
        client.h
        client.cpp
)
target_link_libraries(daisi_network_tcp_client
    PUBLIC
        daisi_network_tcp_framing_manager
        ns3::libcore
        daisi_network_tcp_definitions
    PRIVATE
        daisi_utils
)
target_include_directories(daisi_network_tcp_client
    PUBLIC
        ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_network_tcp_server STATIC)
target_sources(daisi_network_tcp_server
    PRIVATE
        server.h
        server.cpp
)
target_link_libraries(daisi_network_tcp_server
    PUBLIC
        ns3::libcore
        daisi_network_tcp_framing_manager
    PRIVATE
        daisi_utils
)
target_include_directories(daisi_network_tcp_server
    PUBLIC
        ${DAISI_SOURCE_DIR}/src
)
