add_library(NetworkUDPMessage INTERFACE ${SolaNet_SOURCE_DIR}/include/solanet/network_udp/message.h)
target_include_directories(NetworkUDPMessage INTERFACE ${SolaNet_SOURCE_DIR}/include)

add_library(NetworkUDP network_udp.cpp ${PUBLIC_HEADERS} queue.h)
target_include_directories(NetworkUDP PUBLIC ${SolaNet_SOURCE_DIR}/include)
target_link_libraries(NetworkUDP PRIVATE asio Threads::Threads)
