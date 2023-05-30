add_library(daisi_cpps_logical_message_serializer STATIC)
target_sources(daisi_cpps_logical_message_serializer
    PUBLIC
    serializer.cpp
    serializer.h
)
target_link_libraries(daisi_cpps_logical_message_serializer
    PUBLIC
    daisi_cpps_logical_message_call_for_proposal

    PRIVATE
    solanet_serializer
)

add_library(daisi_cpps_logical_message_call_for_proposal INTERFACE)
target_sources(daisi_cpps_logical_message_call_for_proposal
    INTERFACE
    call_for_proposal.h
)
target_link_libraries(daisi_cpps_logical_message_call_for_proposal
    INTERFACE
    solanet_serializer
)

add_library(daisi_cpps_logical_message_bid_submission INTERFACE)
target_sources(daisi_cpps_logical_message_bid_submission
    INTERFACE
    bid_submission.h
)
target_link_libraries(daisi_cpps_logical_message_bid_submission
    INTERFACE
    solanet_serializer
)
