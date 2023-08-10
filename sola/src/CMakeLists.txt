add_subdirectory(storage)
add_subdirectory(event_dissemination)
add_subdirectory(management_overlay)

add_library(SOLAMessage INTERFACE)
target_include_directories(SOLAMessage INTERFACE ${SOLA_SOURCE_DIR}/include)

add_library(sola_service INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/service.h)
target_include_directories(sola_service INTERFACE ${SOLA_SOURCE_DIR}/include)

add_library(SOLA INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/sola.h)
target_include_directories(SOLA INTERFACE ${SOLA_SOURCE_DIR}/include)
target_link_libraries(SOLA INTERFACE
        sola_service
        sola_logger_interface
        solanet_uuid
        solanet_uuid_generator
        NetworkUDP
)

add_library(sola_check INTERFACE)
target_sources(sola_check
        INTERFACE
        ${SOLA_SOURCE_DIR}/src/sola_check.h
)

add_library(sola_logger_interface INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/logger_interface.h)
target_include_directories(sola_logger_interface INTERFACE ${SOLA_SOURCE_DIR}/include)
