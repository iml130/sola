add_library(daisi_cpps_amr_message_serializer serializer.cpp serializer.h)
target_link_libraries(daisi_cpps_amr_message_serializer
    PUBLIC
    daisi_cpps_amr_amr_description
    daisi_cpps_amr_message_amr_status_update
    daisi_cpps_amr_message_amr_order_info
    daisi_cpps_amr_message_amr_order_update
    daisi_cpps_amr_amr_topology

    PRIVATE
    solanet_serializer
)

add_library(daisi_cpps_amr_message_amr_status_update INTERFACE)
target_sources(daisi_cpps_amr_message_amr_status_update
    INTERFACE
    amr_status_update.h
)
target_link_libraries(daisi_cpps_amr_message_amr_status_update INTERFACE
    solanet_serialize
    daisi_structure_helpers
    daisi_cpps_model_order_states
)

add_library(daisi_cpps_amr_message_amr_order_info INTERFACE)
target_sources(daisi_cpps_amr_message_amr_order_info
    INTERFACE
    amr_order_info.h
)
target_link_libraries(daisi_cpps_amr_message_amr_order_info INTERFACE
    solanet_serialize
    daisi_structure_helpers
    daisi_cpps_model_order_states
    daisi_cpps_model_ability
)

add_library(daisi_cpps_amr_message_amr_order_update INTERFACE)
target_sources(daisi_cpps_amr_message_amr_order_update
    INTERFACE
    amr_order_update.h
)
target_link_libraries(daisi_cpps_amr_message_amr_order_update INTERFACE
    solanet_serialize
    daisi_structure_helpers
    daisi_cpps_model_order_states
)

add_library(daisi_cpps_amr_message_amr_state INTERFACE)
target_sources(daisi_cpps_amr_message_amr_state
    INTERFACE
    amr_state.h
)
target_link_libraries(daisi_cpps_amr_message_amr_state INTERFACE
    solanet_serialize
    daisi_structure_helpers
)
