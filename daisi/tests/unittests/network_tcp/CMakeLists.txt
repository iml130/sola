add_executable(daisi_network_tcp_framing_manager_test "")
target_sources(daisi_network_tcp_framing_manager_test
    PRIVATE
        framing_manager_test.cpp
)
target_link_libraries(daisi_network_tcp_framing_manager_test
    PRIVATE
        Catch2::Catch2WithMain
        daisi_network_tcp_framing_manager
)
