add_subdirectory(storage)
add_subdirectory(event_dissemination)
add_subdirectory(management_overlay)

add_library(SOLAMessage INTERFACE)
target_include_directories(SOLAMessage INTERFACE ${SOLA_SOURCE_DIR}/include)

add_library(sola_endpoint INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/endpoint.h)
target_include_directories(sola_endpoint INTERFACE ${SOLA_SOURCE_DIR}/include)

add_library(sola_service INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/service.h)
target_include_directories(sola_service INTERFACE ${SOLA_SOURCE_DIR}/include)
target_link_libraries(sola_service INTERFACE sola_endpoint)

add_library(SOLA INTERFACE ${SOLA_SOURCE_DIR}/include/SOLA/sola.h)
target_include_directories(SOLA INTERFACE ${SOLA_SOURCE_DIR}/include)
target_link_libraries(SOLA INTERFACE sola_service sola_endpoint)

add_library(sola_check INTERFACE)
target_sources(sola_check
        INTERFACE
        ${SOLA_SOURCE_DIR}/src/sola_check.h
)
