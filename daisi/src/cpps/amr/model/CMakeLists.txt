add_library(daisi_cpps_model_load_carrier_ability STATIC)
target_sources(daisi_cpps_model_load_carrier_ability
    PRIVATE
    amr_load_carrier.cpp
    amr_load_carrier.h
)
target_link_libraries(daisi_cpps_model_load_carrier_ability
    PUBLIC
    daisi_cpps_message_serializer
)
target_include_directories(daisi_cpps_model_load_carrier_ability
    PUBLIC
    ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_model_ability STATIC)
target_sources(daisi_cpps_model_ability
    PRIVATE
    amr_static_ability.cpp
    amr_static_ability.h
)
target_link_libraries(daisi_cpps_model_ability
    PUBLIC
    daisi_cpps_model_load_carrier_ability
)
target_include_directories(daisi_cpps_model_ability
    PUBLIC
    ${DAISI_SOURCE_DIR}/src)
