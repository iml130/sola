set(PUBLIC_HEADERS
  ${SolaNet_SOURCE_DIR}/include/solanet/network_udp/message.h
  ${SolaNet_SOURCE_DIR}/include/solanet/network_udp/network_udp.h
)

add_library(NetworkUDP network_udp.cpp ${PUBLIC_HEADERS} queue.h)
set_target_properties(NetworkUDP PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
target_include_directories(NetworkUDP PUBLIC ${SolaNet_SOURCE_DIR}/include)
target_link_libraries(NetworkUDP PRIVATE asio Threads::Threads)
