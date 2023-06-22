add_library(daisi_cpps_model_order_states INTERFACE)
target_sources(daisi_cpps_model_order_states
        INTERFACE
        order_states.h
)

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
