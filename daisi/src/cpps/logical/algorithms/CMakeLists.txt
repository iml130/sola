
add_subdirectory(disposition)

add_library(daisi_cpps_logical_algorithms_algorithm_interface INTERFACE)
target_sources(daisi_cpps_logical_algorithms_algorithm_interface
    INTERFACE
    algorithm_interface.h
)
target_link_libraries(daisi_cpps_logical_algorithms_algorithm_interface
    INTERFACE
    daisi_cpps_logical_message_auction_based_bid_submission
    daisi_cpps_logical_message_auction_based_call_for_proposal
)
