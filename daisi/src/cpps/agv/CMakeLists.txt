add_library(daisi_cpps_agv_agv_physical_basic STATIC)
target_sources(daisi_cpps_agv_agv_physical_basic
        PRIVATE
        agv_physical_basic.h
        agv_physical_basic.cpp
)
target_link_libraries(daisi_cpps_agv_agv_physical_basic
        PUBLIC
        ns3::libcore
        daisi_cpps_model_task
        daisi_cpps_agv_pose
        daisi_cpps_agv_topology
        daisi_cpps_model_kinematics
        daisi_cpps_amr_physical_amr_mobility_model_ns3
        daisi_cpps_agv_agv_states
        daisi_cpps_common_agv_description
        PRIVATE
        daisi_cpps_packet
)
target_include_directories(daisi_cpps_agv_agv_physical_basic
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)
target_compile_definitions(daisi_cpps_agv_agv_physical_basic PRIVATE NS3_LOG_ENABLE)

add_library(daisi_cpps_agv_agv_states INTERFACE)
target_sources(daisi_cpps_agv_agv_states
        INTERFACE
        agv_states.h
)

add_library(daisi_cpps_agv_pose STATIC)
target_sources(daisi_cpps_agv_pose
        PRIVATE
        pose.cpp
        pose.h
)
target_link_libraries(daisi_cpps_agv_pose
        PUBLIC
        ns3::libcore
)
target_include_directories(daisi_cpps_agv_pose
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_agv_topology STATIC)
target_sources(daisi_cpps_agv_topology
        PRIVATE
        topology.cpp
        topology.h
)
target_link_libraries(daisi_cpps_agv_topology
        PUBLIC
        ns3::libcore
)
target_include_directories(daisi_cpps_agv_topology
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)
