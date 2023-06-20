add_library(daisi_cpps_negotiation_task_management_basic_list_task_management STATIC)
target_sources(daisi_cpps_negotiation_task_management_basic_list_task_management
        PRIVATE
        basic_list_task_management.cpp
        basic_list_task_management.h
)
target_link_libraries(daisi_cpps_negotiation_task_management_basic_list_task_management
        PUBLIC
        daisi_cpps_negotiation_task_management_task_management
        daisi_cpps_negotiation_utility_utility_dimensions
)

add_library(daisi_cpps_negotiation_task_management_greedy_tsp_list_task_management STATIC)
target_sources(daisi_cpps_negotiation_task_management_greedy_tsp_list_task_management
        PRIVATE
        greedy_tsp_list_task_management.cpp
        greedy_tsp_list_task_management.h
)
target_link_libraries(daisi_cpps_negotiation_task_management_greedy_tsp_list_task_management
        PUBLIC
        daisi_cpps_negotiation_task_management_basic_list_task_management
)

add_library(daisi_cpps_negotiation_task_management_stn_task_management STATIC)
target_sources(daisi_cpps_negotiation_task_management_stn_task_management
        PRIVATE
        stn_task_management.cpp
        stn_task_management.h
)
target_link_libraries(daisi_cpps_negotiation_task_management_stn_task_management
        PUBLIC
        daisi_cpps_negotiation_task_management_task_management
        daisi_cpps_negotiation_utils_simple_temporal_network
)

add_library(daisi_cpps_negotiation_task_management_task_management STATIC)
target_sources(daisi_cpps_negotiation_task_management_task_management
        PRIVATE
        task_management.cpp
        task_management.h
)
target_link_libraries(daisi_cpps_negotiation_task_management_task_management
        PUBLIC
        daisi_cpps_negotiation_task_management_insert_info
        ns3::libcore
        daisi_cpps_model_kinematics
        daisi_cpps_amr_model_amr_static_ability
        daisi_cpps_model_task
        daisi_cpps_common_cpps_logger_ns3
)

add_library(daisi_cpps_negotiation_task_management_insert_info INTERFACE)
target_sources(daisi_cpps_negotiation_task_management_insert_info
        INTERFACE
        insert_info.h
)
target_link_libraries(daisi_cpps_negotiation_task_management_insert_info
        INTERFACE
        daisi_cpps_negotiation_utils_simple_temporal_network_components
)
