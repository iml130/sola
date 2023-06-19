add_library(daisi_cpps_amr_amr_load_carrier STATIC)
target_sources(daisi_cpps_amr_amr_load_carrier
    PUBLIC
    amr_load_carrier.cpp
    amr_load_carrier.h
)
target_link_libraries(daisi_cpps_amr_amr_load_carrier
    PUBLIC
    solanet_serialize
)
target_include_directories(daisi_cpps_amr_amr_load_carrier
    PUBLIC
    ${DAISI_SOURCE_DIR}/src)

add_library(daisi_cpps_amr_amr_static_ability STATIC)
target_sources(daisi_cpps_amr_amr_static_ability
    PUBLIC
    amr_static_ability.cpp
    amr_static_ability.h
)
target_link_libraries(daisi_cpps_amr_amr_static_ability
    PUBLIC
    daisi_cpps_amr_amr_load_carrier
    solanet_serialize
)
target_include_directories(daisi_cpps_amr_amr_static_ability
    PUBLIC
    ${DAISI_SOURCE_DIR}/src)
