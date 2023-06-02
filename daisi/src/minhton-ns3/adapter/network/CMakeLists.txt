# Network facade sim
add_library(minhton_network_facade_sim STATIC)
target_sources(minhton_network_facade_sim
  PRIVATE
  ${MINHTON_SOURCE_DIR}/src/network/network_facade.cpp
    ${MINHTON_SOURCE_DIR}/include/minhton/network/network_facade.h
)
target_link_libraries(minhton_network_facade_sim 
  PUBLIC
    NetworkUDPSim
    minhton_message_sim
    minhton_utils_serializer_cereal_sim
)
