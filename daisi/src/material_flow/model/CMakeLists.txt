add_library(daisi_material_flow_model_location STATIC)
target_sources(daisi_material_flow_model_location
    PRIVATE
    location.h
    location.cpp
)
target_link_libraries(daisi_material_flow_model_location
    PUBLIC
    daisi_structure_helpers
)
target_include_directories(daisi_material_flow_model_location
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)

add_library(daisi_material_flow_model_action_order_step STATIC)
target_sources(daisi_material_flow_model_action_order_step
    PRIVATE
    action_order_step.h
    action_order_step.cpp
)
target_include_directories(daisi_material_flow_model_action_order_step
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
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
)
target_include_directories(daisi_material_flow_model_action_order
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
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
)
target_include_directories(daisi_material_flow_model_move_order_step
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
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
)
target_include_directories(daisi_material_flow_model_move_order
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
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
)
target_include_directories(daisi_material_flow_model_transport_order_step
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
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
)
target_include_directories(daisi_material_flow_model_transport_order
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
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
)
target_include_directories(daisi_material_flow_model_task
    PUBLIC
    ${DAISI_SOURCE_DIR}/src
)
