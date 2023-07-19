add_library(daisi_logging_definitions INTERFACE definitions.h)
target_include_directories(daisi_logging_definitions INTERFACE ${DAISI_SOURCE_DIR}/src)

add_library(daisi_logging_sqlite_helper STATIC)

target_sources(daisi_logging_sqlite_helper
    PRIVATE
    sqlite/sqlite_helper.cpp
    sqlite/sqlite_helper.h
)

if(DAISI_SQLITE_DEFERRED_LOGGING)
    target_compile_definitions(daisi_logging_sqlite_helper PRIVATE DEFERRED_LOGGING)
endif()

target_link_libraries(daisi_logging_sqlite_helper
    PRIVATE
    SQLite::SQLite3
    daisi_utils
)

target_include_directories(daisi_logging_sqlite_helper
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

add_library(daisi_logger_manager logger_manager.h logger_manager.cpp)
target_link_libraries(daisi_logger_manager
    PUBLIC
    daisi_logging_sqlite_helper
    daisi_minhton_logger_ns3
    daisi_natter_logger_ns3
    daisi_cpps_common_cpps_logger_ns3
    #PathPlanning
    daisi_sola_logger_ns3
    PRIVATE
    ns3::libcore
)
