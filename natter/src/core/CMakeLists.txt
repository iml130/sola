add_library(natter_network_facade STATIC)
target_sources(natter_network_facade
  PRIVATE
    network_facade.h
)
set_target_properties(natter_network_facade PROPERTIES LINKER_LANGUAGE CXX)

add_library(natter_network_info INTERFACE)
target_sources(natter_network_info
        INTERFACE
        ${natter_SOURCE_DIR}/include/natter/network_info_ipv4.h
)

target_link_libraries(natter_network_facade
  PUBLIC
    solanet_serializer
    NetworkUDP
)

add_library(natter_assert INTERFACE)
target_sources(natter_assert
        INTERFACE
        natter_check.h
)
