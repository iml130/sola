add_subdirectory(network_udp)
add_subdirectory(uuid)

add_library(solanet_serializer INTERFACE)
target_sources(solanet_serializer
        INTERFACE
        ${SolaNet_SOURCE_DIR}/include/solanet/serializer/serializer.h
)
target_link_libraries(solanet_serializer INTERFACE Cereal)
target_include_directories(solanet_serializer INTERFACE ${SolaNet_SOURCE_DIR}/include)

add_library(solanet_serialize INTERFACE)
target_sources(solanet_serialize
        INTERFACE
        ${SolaNet_SOURCE_DIR}/include/solanet/serializer/serialize.h
)
target_include_directories(solanet_serialize INTERFACE ${SolaNet_SOURCE_DIR}/include)
