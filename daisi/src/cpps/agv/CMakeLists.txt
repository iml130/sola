

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
