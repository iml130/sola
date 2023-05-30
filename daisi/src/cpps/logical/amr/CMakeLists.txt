add_library(daisi_cpps_logical_amr_amr_logical_agent STATIC)
target_sources(daisi_cpps_logical_amr_amr_logical_agent
    PUBLIC
    amr_logical_agent.cpp
    amr_logical_agent.h
)
target_link_libraries(daisi_cpps_logical_amr_amr_logical_agent
    PUBLIC
)
