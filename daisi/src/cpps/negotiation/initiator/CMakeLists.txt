add_library(daisi_cpps_task_allocation_initiator_iterated_auction_initiator INTERFACE)
target_sources(daisi_cpps_task_allocation_initiator_iterated_auction_initiator
        INTERFACE
        iterated_auction_initiator.h
        iterated_auction_initiator.cpp
        )
target_link_libraries(daisi_cpps_task_allocation_initiator_iterated_auction_initiator
        INTERFACE
        daisi_cpps_task_allocation_initiator_task_allocation_initiator
        daisi_sola_sola_ns3_wrapper
        daisi_cpps_negotiation_utils_overload
        daisi_cpps_negotiation_utils_simple_temporal_network
        daisi_cpps_negotiation_utils_kmeans
        ns3::libcore
        )
target_include_directories(daisi_cpps_task_allocation_initiator_iterated_auction_initiator 
        INTERFACE 
        ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_cpps_task_allocation_initiator_iterated_auction_initiator_pubsub_modified STATIC)
target_sources(daisi_cpps_task_allocation_initiator_iterated_auction_initiator_pubsub_modified
        PRIVATE
        iterated_auction_initiator_pubsub_modified.cpp
        iterated_auction_initiator_pubsub_modified.h
        )
target_link_libraries(daisi_cpps_task_allocation_initiator_iterated_auction_initiator_pubsub_modified
        PUBLIC
        daisi_cpps_task_allocation_initiator_iterated_auction_initiator
        )

add_library(daisi_cpps_task_allocation_initiator_iterated_auction_initiator_pubsub_tepssi STATIC)
target_sources(daisi_cpps_task_allocation_initiator_iterated_auction_initiator_pubsub_tepssi
        PRIVATE
        iterated_auction_initiator_pubsub_tepssi.cpp
        iterated_auction_initiator_pubsub_tepssi.h
        )
target_link_libraries(daisi_cpps_task_allocation_initiator_iterated_auction_initiator_pubsub_tepssi
        PUBLIC
        daisi_cpps_task_allocation_initiator_iterated_auction_initiator
        )

add_library(daisi_cpps_task_allocation_initiator_task_allocation_initiator INTERFACE)
target_sources(daisi_cpps_task_allocation_initiator_task_allocation_initiator
        INTERFACE
        task_allocation_initiator.h
        )
target_link_libraries(daisi_cpps_task_allocation_initiator_task_allocation_initiator
        INTERFACE
        daisi_sola_sola_ns3_wrapper
        daisi_cpps_model_task
        daisi_cpps_message_serializer
        daisi_cpps_model_material_flow_model
        )
