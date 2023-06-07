add_library(daisi_cpps_logical_message_auction_based_call_for_proposal INTERFACE)
target_sources(daisi_cpps_logical_message_auction_based_call_for_proposal
    INTERFACE
    call_for_proposal.h
)
target_link_libraries(daisi_cpps_logical_message_auction_based_call_for_proposal
    INTERFACE
    solanet_serializer
)

add_library(daisi_cpps_logical_message_auction_based_bid_submission INTERFACE)
target_sources(daisi_cpps_logical_message_auction_based_bid_submission
    INTERFACE
    bid_submission.h
)
target_link_libraries(daisi_cpps_logical_message_auction_based_bid_submission
    INTERFACE
    solanet_serializer
)