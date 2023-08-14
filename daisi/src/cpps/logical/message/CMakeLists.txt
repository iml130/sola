add_subdirectory(auction_based)
add_subdirectory(central_allocation)

add_library(daisi_cpps_logical_message_serializer STATIC)
target_sources(daisi_cpps_logical_message_serializer
    PRIVATE
    serializer.cpp
    serializer.h
)
target_link_libraries(daisi_cpps_logical_message_serializer
    PUBLIC
    daisi_cpps_logical_message_auction_based_call_for_proposal
    daisi_cpps_logical_message_auction_based_bid_submission
    daisi_cpps_logical_message_auction_based_iteration_notification
    daisi_cpps_logical_message_auction_based_winner_notification
    daisi_cpps_logical_message_auction_based_winner_response
    daisi_cpps_logical_message_central_allocation_assignment_notification
    daisi_cpps_logical_message_central_allocation_assignment_response
    daisi_cpps_logical_message_central_allocation_status_update
    daisi_cpps_logical_message_central_allocation_status_update_request
    daisi_cpps_logical_message_material_flow_update

    PRIVATE
    solanet_serializer
)
target_include_directories(daisi_cpps_logical_message_serializer
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_cpps_logical_message_material_flow_update INTERFACE)
target_sources(daisi_cpps_logical_message_material_flow_update
    INTERFACE
    material_flow_update.h
)
target_link_libraries(daisi_cpps_logical_message_material_flow_update
    INTERFACE
    solanet_serializer
    daisi_material_flow_model_task
    daisi_cpps_model_order_states
    daisi_structure_helpers
)
