include(create_ns3_executable)

add_subdirectory(amr)
add_subdirectory(model)
add_subdirectory(common)
add_subdirectory(logical)

add_library(daisi_cpps_packet STATIC)
target_sources(daisi_cpps_packet
        PRIVATE
        packet.cpp
        packet.h
)
target_link_libraries(daisi_cpps_packet
        PUBLIC
        ns3::libcore
)

create_ns3_executable(
        daisi::cpps::CppsManager # Class
        cpps/common/cpps_manager.h # Include
        daisi_cpps_common_cpps_manager # CMake target
        CPPS # Name
)
