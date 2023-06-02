add_subdirectory(initiator)
add_subdirectory(task_management)
add_subdirectory(participant)
add_subdirectory(utility)
add_subdirectory(utils)

add_library(daisi_cpps_negotiation_mrta_configuration INTERFACE)
target_sources(daisi_cpps_negotiation_mrta_configuration
        INTERFACE
        mrta_configuration.h
        )
