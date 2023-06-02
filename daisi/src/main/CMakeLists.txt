add_executable(Main daisi_main.cpp)

target_link_libraries(Main
        PRIVATE
            ns3::libcore
            daisi_minhton_manager
            daisi_natter_manager
            daisi_utils
            daisi_cpps_common_cpps_manager
            PathPlanning
            daisi_sola_manager
)
