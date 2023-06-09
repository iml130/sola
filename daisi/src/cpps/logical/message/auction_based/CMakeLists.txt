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
    daisi_cpps_model_ability
)

add_library(daisi_cpps_logical_message_auction_based_iteration_notification INTERFACE)
target_sources(daisi_cpps_logical_message_auction_based_iteration_notification
    INTERFACE
    iteration_notification.h
)
target_link_libraries(daisi_cpps_logical_message_auction_based_iteration_notification
    INTERFACE
    solanet_serializer
)

add_library(daisi_cpps_logical_message_auction_based_winner_notification INTERFACE)
target_sources(daisi_cpps_logical_message_auction_based_winner_notification
    INTERFACE
    winner_notification.h
)
target_link_libraries(daisi_cpps_logical_message_auction_based_winner_notification
    INTERFACE
    solanet_serializer
)

add_library(daisi_cpps_logical_message_auction_based_winner_response INTERFACE)
target_sources(daisi_cpps_logical_message_auction_based_winner_response
    INTERFACE
    winner_response.h
)
target_link_libraries(daisi_cpps_logical_message_auction_based_winner_response
    INTERFACE
    solanet_serializer
)