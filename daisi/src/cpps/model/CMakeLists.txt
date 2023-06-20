add_library(daisi_cpps_model_task STATIC)
target_sources(daisi_cpps_model_task
        PRIVATE
        task.cpp
        task.h
)
target_link_libraries(daisi_cpps_model_task
        PUBLIC
        ns3::libcore
        daisi_cpps_model_order_states
        daisi_cpps_amr_model_amr_static_ability
        daisi_cpps_model_kinematics
        daisi_cpps_model_agv_fleet
        daisi_cpps_model_time_window
        daisi_cpps_model_precedence_constraint
        daisi_cpps_model_material_flow_model
        daisi_cpps_message_serializer
        PRIVATE
        daisi_cpps_common_uuid_generator
)

add_library(daisi_cpps_model_order_states INTERFACE)
target_sources(daisi_cpps_model_order_states
        INTERFACE
        order_states.h
)

add_library(daisi_cpps_model_time_window INTERFACE)
target_sources(daisi_cpps_model_time_window
        INTERFACE
        time_window.h
)
target_link_libraries(daisi_cpps_model_time_window
        INTERFACE
        daisi_cpps_message_serializer
)

add_library(daisi_cpps_model_precedence_constraint INTERFACE)
target_sources(daisi_cpps_model_precedence_constraint
        INTERFACE
        precedence_constraint.h
)
target_link_libraries(daisi_cpps_model_precedence_constraint
        INTERFACE
        daisi_cpps_message_serializer
)

add_library(daisi_cpps_model_material_flow_model STATIC)
target_sources(daisi_cpps_model_material_flow_model
        PRIVATE
        material_flow_model.h
        material_flow_model.cpp
)
target_link_libraries(daisi_cpps_model_material_flow_model
        PUBLIC
        ns3::libcore
        daisi_cpps_negotiation_utils_precedence_graph
        daisi_cpps_model_task
        PRIVATE
        daisi_random_engine
)
target_include_directories(daisi_cpps_model_material_flow_model
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_model_kinematics STATIC)
target_sources(daisi_cpps_model_kinematics
        PRIVATE
        kinematics.cpp
        kinematics.h
)
target_link_libraries(daisi_cpps_model_kinematics
        PUBLIC
        ns3::libcore
        daisi_cpps_message_serializer
)
target_include_directories(daisi_cpps_model_kinematics
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_model_agv_fleet STATIC)
target_sources(daisi_cpps_model_agv_fleet
        PRIVATE
        agv_fleet.cpp
        agv_fleet.h
)
target_link_libraries(daisi_cpps_model_agv_fleet
        PUBLIC
        daisi_cpps_model_kinematics
        daisi_cpps_amr_model_amr_static_ability
)
target_include_directories(daisi_cpps_model_agv_fleet
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)
