add_library(daisi_material_flow_model_location STATIC)
target_sources(daisi_material_flow_model_location
    PRIVATE
    location.h
    location.cpp
)
target_link_libraries(daisi_material_flow_model_location
    PUBLIC
    daisi_structure_helpers

    PRIVATE
    solanet_serialize
)

add_library(daisi_material_flow_model_action_order_step STATIC)
target_sources(daisi_material_flow_model_action_order_step
    PRIVATE
    action_order_step.h
    action_order_step.cpp
)
target_link_libraries(daisi_material_flow_model_action_order_step
    PRIVATE
    solanet_serializer
)

add_library(daisi_material_flow_model_action_order STATIC)
target_sources(daisi_material_flow_model_action_order
    PRIVATE
    action_order.h
    action_order.cpp
)
target_link_libraries(daisi_material_flow_model_action_order
    PUBLIC
    daisi_material_flow_model_action_order_step

    PRIVATE
    solanet_serialize
)

add_library(daisi_material_flow_model_move_order_step STATIC)
target_sources(daisi_material_flow_model_move_order_step
    PRIVATE
    move_order_step.h
    move_order_step.cpp
)
target_link_libraries(daisi_material_flow_model_move_order_step
    PUBLIC
    daisi_material_flow_model_location

    PRIVATE
    solanet_serialize
)

add_library(daisi_material_flow_model_move_order STATIC "")
target_sources(daisi_material_flow_model_move_order
    PRIVATE
    move_order.h
    move_order.cpp
)
target_link_libraries(daisi_material_flow_model_move_order
    PUBLIC
    daisi_material_flow_model_move_order_step

    PRIVATE
    solanet_serialize
)

add_library(daisi_material_flow_model_transport_order_step STATIC "")
target_sources(daisi_material_flow_model_transport_order_step
    PRIVATE
    transport_order_step.h
    transport_order_step.cpp
)
target_link_libraries(daisi_material_flow_model_transport_order_step
    PUBLIC
    daisi_material_flow_model_location

    PRIVATE
    solanet_serialize
)

add_library(daisi_material_flow_model_transport_order STATIC "")
target_sources(daisi_material_flow_model_transport_order
    PRIVATE
    transport_order.h
    transport_order.cpp
)
target_link_libraries(daisi_material_flow_model_transport_order
    PUBLIC
    daisi_material_flow_model_transport_order_step

    PRIVATE
    solanet_serialize
)

add_library(daisi_material_flow_model_task STATIC "")
target_sources(daisi_material_flow_model_task
    PRIVATE
    task.h
    task.cpp
)
target_link_libraries(daisi_material_flow_model_task
    PUBLIC
    daisi_material_flow_model_transport_order
    daisi_material_flow_model_move_order
    daisi_material_flow_model_action_order
    daisi_cpps_amr_model_amr_static_ability

    PRIVATE
    solanet_serialize
    daisi_cpps_common_uuid_generator
)

add_library(daisi_material_flow_model_material_flow STATIC "")
target_sources(daisi_material_flow_model_material_flow
    PRIVATE
    material_flow.h
    material_flow.cpp
)
target_link_libraries(daisi_material_flow_model_material_flow
    PUBLIC
    daisi_material_flow_model_task

    PRIVATE
    solanet_serialize
)
