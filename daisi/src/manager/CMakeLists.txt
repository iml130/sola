add_library(daisi_manager_scenariofileparser STATIC)

target_sources(daisi_manager_scenariofileparser
    PRIVATE
        scenariofileparser.cpp
        scenariofileparser.h
)

target_link_libraries(daisi_manager_scenariofileparser
    PUBLIC
        yaml-cpp
)



add_library(daisi_manager INTERFACE)

target_sources(daisi_manager
       INTERFACE
           sola_constants.h
           sola_helper.h
           manager.h
)

target_link_libraries(daisi_manager
        INTERFACE
            daisi_manager_scenariofileparser
            daisi_logger_manager
            ns3::libnetwork
            ns3::libbridge
            ns3::libcsma
            ns3::libinternet
            daisi_random_engine
)

target_include_directories(daisi_manager
        INTERFACE
            ${PROJECT_SOURCE_DIR}/src
        )
