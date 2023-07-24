add_library(natter_minhcast_obj OBJECT "")
target_sources(natter_minhcast_obj
    PRIVATE
        minhcast_impl.h
        minhcast_impl.cpp
)
target_include_directories(natter_minhcast_obj
        PUBLIC
            ${natter_SOURCE_DIR}/include # For public headers
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(natter_minhcast_obj # Only populates include directories and does no linking
        PRIVATE
            natter_network_facade
            natter_utils
            natter_logging
        PUBLIC
            natter_interface
)

add_library(natter_minhcast STATIC)
target_sources(natter_minhcast
        PRIVATE
        $<TARGET_OBJECTS:natter_minhcast_obj>
        INTERFACE
        ${natter_SOURCE_DIR}/include/natter/natter_minhcast.h
        minhcast_message.h
        forwarding_limit.h
        broadcast_info.h
        )
target_include_directories(natter_minhcast
        PUBLIC
            ${natter_SOURCE_DIR}/include # For public headers
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(natter_minhcast
        PRIVATE
            natter_network_facade
            natter_utils
            natter_logging
        PUBLIC
            natter_interface
            solanet_serialize
            solanet_uuid
            solanet_uuid_generator
)

