add_subdirectory(auction_based)

add_library(daisi_cpps_logical_message_serializer STATIC)
target_sources(daisi_cpps_logical_message_serializer
    PUBLIC
    serializer.cpp
    serializer.h
)
target_link_libraries(daisi_cpps_logical_message_serializer
    PUBLIC
    daisi_cpps_logical_message_auction_based_call_for_proposal
    daisi_cpps_logical_message_auction_based_bid_submission

    PRIVATE
    solanet_serializer
)
target_include_directories(daisi_cpps_logical_message_serializer
    PUBLIC
    ${DAISI_SOURCE_DIR}/src)
