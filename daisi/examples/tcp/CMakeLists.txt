add_executable(daisi_tcp_example tcp_example.cpp)
target_link_libraries(daisi_tcp_example
    PRIVATE
        ns3::libcore
        daisi_network_tcp_server
        daisi_network_tcp_client
        daisi_utils
        daisi_socket_manager
        ns3::libcsma
)
target_include_directories(daisi_tcp_example
    PRIVATE
        ${DAISI_SOURCE_DIR}/src
)
