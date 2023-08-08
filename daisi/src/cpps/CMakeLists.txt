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

add_library(daisi_cpps_sola_wrapper INTERFACE sola_wrapper.h)
target_link_libraries(daisi_cpps_sola_wrapper
        INTERFACE
        ManagementOverlayMINHTONSim
        EventDisseminationMinhcastSim
        SOLA_ns3
)
