add_library(daisi_cpps_task_allocation_participant_iterated_auction_participant STATIC)
target_sources(daisi_cpps_task_allocation_participant_iterated_auction_participant
        PRIVATE
        iterated_auction_participant.cpp
        iterated_auction_participant.h
        )
target_link_libraries(daisi_cpps_task_allocation_participant_iterated_auction_participant
        PUBLIC
        daisi_sola_sola_ns3_wrapper
        daisi_cpps_task_allocation_participant_task_allocation_participant
        daisi_cpps_model_task
        daisi_cpps_negotiation_utils_overload
        )

add_library(daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_tepssi STATIC)
target_sources(daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_tepssi
        PRIVATE
        iterated_auction_participant_pubsub_tepssi.cpp
        iterated_auction_participant_pubsub_tepssi.h
        )
target_link_libraries(daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_tepssi
        PUBLIC
        daisi_cpps_task_allocation_participant_iterated_auction_participant
        )

add_library(daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_modified STATIC)
target_sources(daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_modified
        PRIVATE
        iterated_auction_participant_pubsub_modified.cpp
        iterated_auction_participant_pubsub_modified.h
        )
target_link_libraries(daisi_cpps_task_allocation_participant_iterated_auction_participant_pubsub_modified
        PUBLIC
        daisi_cpps_task_allocation_participant_iterated_auction_participant
        )

add_library(daisi_cpps_task_allocation_participant_task_allocation_participant INTERFACE)
target_sources(daisi_cpps_task_allocation_participant_task_allocation_participant
        INTERFACE
        task_allocation_participant.h
        )
target_link_libraries(daisi_cpps_task_allocation_participant_task_allocation_participant
        INTERFACE
        daisi_sola_sola_ns3_wrapper
        daisi_cpps_negotiation_task_management_task_management
        daisi_cpps_message_serializer
)
