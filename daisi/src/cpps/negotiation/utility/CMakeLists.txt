add_library(daisi_cpps_negotiation_utility_utility_dimensions STATIC)
target_sources(daisi_cpps_negotiation_utility_utility_dimensions
        PRIVATE
        utility_dimensions.cpp
        utility_dimensions.h
        )
target_link_libraries(daisi_cpps_negotiation_utility_utility_dimensions
        PUBLIC
        daisi_cpps_message_serializer
        daisi_cpps_model_task
        )
target_include_directories(daisi_cpps_negotiation_utility_utility_dimensions
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utility_accumulated_utility_dimensions STATIC)
target_sources(daisi_cpps_negotiation_utility_accumulated_utility_dimensions
        PRIVATE
        accumulated_utility_dimensions.h
        )
target_link_libraries(daisi_cpps_negotiation_utility_accumulated_utility_dimensions
        PUBLIC
        daisi_cpps_negotiation_utility_utility_dimensions
        daisi_cpps_message_serializer
        )
target_include_directories(daisi_cpps_negotiation_utility_accumulated_utility_dimensions
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_negotiation_utility_utility_evaluator STATIC)
target_sources(daisi_cpps_negotiation_utility_utility_evaluator
        PRIVATE
        utility_evaluator.cpp
        utility_evaluator.h
        )
target_link_libraries(daisi_cpps_negotiation_utility_utility_evaluator
        PUBLIC
        daisi_cpps_negotiation_utility_utility_dimensions
        )
        target_include_directories(daisi_cpps_negotiation_utility_utility_evaluator
        PUBLIC
        ${DAISI_SOURCE_DIR}/src)


