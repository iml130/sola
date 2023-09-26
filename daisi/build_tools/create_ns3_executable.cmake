function(create_ns3_executable MANAGER MANAGER_INCLUDE MANAGER_LIB APP_NAME)
    configure_file(${DAISI_SOURCE_DIR}/src/utils/daisi_main.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}_main.cpp)
    add_executable(${APP_NAME}_ns3 ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}_main.cpp)
    target_link_libraries(${APP_NAME}_ns3
        PRIVATE
            ns3::libcore
            daisi_utils
            ${MANAGER_LIB}
    )
endfunction()
