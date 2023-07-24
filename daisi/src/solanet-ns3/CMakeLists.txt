# Build NetworkUDP for simulation
add_library(NetworkUDPSim STATIC)
target_sources(NetworkUDPSim
    PRIVATE
    ${SolaNet_SOURCE_DIR}/include/solanet/network_udp/network_udp.h
    ns3SolaNet.cpp
)

target_link_libraries(NetworkUDPSim
    PRIVATE
    ns3::libcore
    daisi_utils
    daisi_solanet_message_ns3
    daisi_socket_manager
)
target_include_directories(NetworkUDPSim
    PUBLIC
    ${SolaNet_SOURCE_DIR}/include/)

add_library(daisi_solanet_message_ns3 STATIC)
target_sources(daisi_solanet_message_ns3
        PRIVATE
            sola_message_ns3.h
            sola_message_ns3.cpp
)
target_link_libraries(daisi_solanet_message_ns3
        PUBLIC
            ns3::libcore
)
target_include_directories(daisi_solanet_message_ns3
        PUBLIC
            ${PROJECT_SOURCE_DIR}/src)

if(DAISI_DISABLE_NETWORK_SIMULATION)
    target_compile_definitions(NetworkUDPSim PRIVATE DAISI_SOLANET_NS3_DISABLE_NETWORKING)
endif()

add_library(solanet_uuid_generator_sim STATIC)
target_sources(solanet_uuid_generator_sim
  PRIVATE
  ${SolaNet_SOURCE_DIR}/include/solanet/uuid_generator.h
  uuid_generator_sim.cpp
)
target_link_libraries(solanet_uuid_generator_sim PRIVATE daisi_random_engine ${LIBUUID})
target_include_directories(solanet_uuid_generator_sim
        PRIVATE
            ${SolaNet_SOURCE_DIR}/include
)
