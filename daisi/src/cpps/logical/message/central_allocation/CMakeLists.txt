add_library(daisi_cpps_logical_message_central_allocation_assignment_notification INTERFACE)
target_sources(daisi_cpps_logical_message_central_allocation_assignment_notification
    INTERFACE
    assignment_notification.h
)
target_link_libraries(daisi_cpps_logical_message_central_allocation_assignment_notification
    INTERFACE
    solanet_serializer
    daisi_material_flow_model_task
    solanet_uuid
    solanet_uuid_generator_sim
)

add_library(daisi_cpps_logical_message_central_allocation_assignment_response INTERFACE)
target_sources(daisi_cpps_logical_message_central_allocation_assignment_response
    INTERFACE
    assignment_response.h
)
target_link_libraries(daisi_cpps_logical_message_central_allocation_assignment_response
    INTERFACE
    solanet_serializer
    daisi_cpps_logical_task_management_metrics_composition
    daisi_structure_helpers
    solanet_uuid
    solanet_uuid_generator_sim
)

add_library(daisi_cpps_logical_message_central_allocation_status_update_request INTERFACE)
target_sources(daisi_cpps_logical_message_central_allocation_status_update_request
    INTERFACE
    status_update_request.h
)
target_link_libraries(daisi_cpps_logical_message_central_allocation_status_update_request
    INTERFACE
    solanet_serializer
    solanet_uuid
    solanet_uuid_generator_sim
)

add_library(daisi_cpps_logical_message_central_allocation_status_update INTERFACE)
target_sources(daisi_cpps_logical_message_central_allocation_status_update
    INTERFACE
    status_update.h
)
target_link_libraries(daisi_cpps_logical_message_central_allocation_status_update
    INTERFACE
    solanet_serializer
    daisi_cpps_logical_task_management_metrics_composition
    daisi_structure_helpers
    solanet_uuid
    solanet_uuid_generator_sim
)
