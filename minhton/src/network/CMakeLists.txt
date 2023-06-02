add_library(minhton_network_facade STATIC)

target_sources(minhton_network_facade
  PRIVATE
    network_facade.cpp
    ${MINHTON_SOURCE_DIR}/include/minhton/network/network_facade.h
)

target_link_libraries(minhton_network_facade 
  PUBLIC
    NetworkUDP
    minhton_message
    minhton_utils_serializer_cereal
)
