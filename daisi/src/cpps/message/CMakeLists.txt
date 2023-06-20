add_library(daisi_cpps_message_ssi_call_for_proposal STATIC)
target_sources(daisi_cpps_message_ssi_call_for_proposal
        PRIVATE
        ssi_call_for_proposal.cpp
        ssi_call_for_proposal.h
)
target_link_libraries(daisi_cpps_message_ssi_call_for_proposal
        PUBLIC
        ns3::libcore
        daisi_cpps_model_task
        solanet_serialize
)
target_include_directories(daisi_cpps_message_ssi_call_for_proposal
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_ssi_iteration_notification STATIC)
target_sources(daisi_cpps_message_ssi_iteration_notification
        PRIVATE
        ssi_iteration_notification.cpp
        ssi_iteration_notification.h
)
target_link_libraries(daisi_cpps_message_ssi_iteration_notification
        PUBLIC
        ns3::libcore
        solanet_serialize
)
target_include_directories(daisi_cpps_message_ssi_iteration_notification
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_ssi_winner_notification STATIC)
target_sources(daisi_cpps_message_ssi_winner_notification
        PRIVATE
        ssi_winner_notification.cpp
        ssi_winner_notification.h
)
target_link_libraries(daisi_cpps_message_ssi_winner_notification
        PUBLIC
        ns3::libcore
        solanet_serialize
)
target_include_directories(daisi_cpps_message_ssi_winner_notification
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_ssi_submission STATIC)
target_sources(daisi_cpps_message_ssi_submission
        PRIVATE
        ssi_submission.cpp
        ssi_submission.h
)
target_link_libraries(daisi_cpps_message_ssi_submission
        PUBLIC
        ns3::libcore
        daisi_cpps_amr_model_amr_static_ability
        solanet_serialize
)
target_include_directories(daisi_cpps_message_ssi_submission
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_ssi_winner_response STATIC)
target_sources(daisi_cpps_message_ssi_winner_response
        PRIVATE
        ssi_winner_response.cpp
        ssi_winner_response.h
)
target_link_libraries(daisi_cpps_message_ssi_winner_response
        PUBLIC
        ns3::libcore
        solanet_serialize
        daisi_cpps_model_task
)
target_include_directories(daisi_cpps_message_ssi_submission
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_serializer STATIC)
target_sources(daisi_cpps_message_serializer
        PRIVATE
        serializer.cpp
        serializer.h
)
target_link_libraries(daisi_cpps_message_serializer
        PUBLIC
        solanet_serialize
        daisi_cpps_message_types_all
        PRIVATE
        Cereal
)
target_include_directories(daisi_cpps_message_serializer
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_task_update STATIC)
target_sources(daisi_cpps_message_task_update
        PRIVATE
        task_update.cpp
        task_update.h
)
target_link_libraries(daisi_cpps_message_task_update
        PUBLIC
        ns3::libcore
        solanet_serialize
        daisi_cpps_model_task
)
target_include_directories(daisi_cpps_message_task_update
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_task_info STATIC)
target_sources(daisi_cpps_message_task_info
        PRIVATE
        task_info.cpp
        task_info.h
)
target_link_libraries(daisi_cpps_message_task_info
        PUBLIC
        ns3::libcore
        solanet_serialize
        daisi_cpps_model_task
)
target_include_directories(daisi_cpps_message_task_info
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_message_types_all INTERFACE)
target_sources(daisi_cpps_message_types_all
        INTERFACE
        types_all.h
)
target_link_libraries(daisi_cpps_message_types_all
        INTERFACE
        daisi_cpps_message_ssi_call_for_proposal
        daisi_cpps_message_ssi_submission
        daisi_cpps_message_ssi_winner_notification
        daisi_cpps_message_ssi_winner_response
        daisi_cpps_message_ssi_iteration_notification
        daisi_cpps_message_task_update
        daisi_cpps_message_task_info
        daisi_cpps_amr_model_amr_static_ability
)
target_include_directories(daisi_cpps_message_types_all
        INTERFACE
        ${DAISI_SOURCE_DIR}/src)
