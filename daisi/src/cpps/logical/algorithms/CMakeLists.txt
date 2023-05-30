
add_subdirectory(disposition)

add_library(daisi_cpps_logical_algorithms_algorithm_interface INTERFACE)
target_sources(daisi_cpps_logical_algorithms_algorithm_interface
    INTERFACE
    algorithm_interface.h
)
target_link_libraries(daisi_cpps_logical_algorithms_algorithm_interface
    INTERFACE
)
