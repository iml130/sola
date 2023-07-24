add_library(solanet_uuid
    STATIC
    ${SolaNet_SOURCE_DIR}/include/solanet/uuid.h
    uuid.cpp
)
target_include_directories(solanet_uuid PUBLIC ${SolaNet_SOURCE_DIR}/include)
target_link_libraries(solanet_uuid PRIVATE ${LIBUUID})

add_library(solanet_uuid_generator
    STATIC
    ${SolaNet_SOURCE_DIR}/include/solanet/uuid_generator.h
    uuid_generator.cpp
)
target_include_directories(solanet_uuid_generator PUBLIC ${SolaNet_SOURCE_DIR}/include)
target_link_libraries(solanet_uuid_generator
    PUBLIC
    solanet_uuid
    PRIVATE
    ${LIBUUID}
)
