add_library(daisi_cpps_logical_amr_amr_logical_agent STATIC)
target_sources(daisi_cpps_logical_amr_amr_logical_agent PUBLIC
    amr_logical_agent.cpp
    amr_logical_agent.h
)
target_link_libraries(daisi_cpps_logical_amr_amr_logical_agent
    PUBLIC
    daisi_cpps_logical_logical_agent
    daisi_cpps_logical_algorithms_disposition_iterated_auction_disposition_participant
    daisi_cpps_amr_message_serializer
    ns3::libcore
    daisi_cpps_amr_physical_material_flow_functionality_mapping
    daisi_cpps_logical_order_management_order_management
    daisi_cpps_logical_order_management_stn_order_management
    daisi_utils
)
