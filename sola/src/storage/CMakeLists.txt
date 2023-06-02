add_library(Storage INTERFACE)
target_include_directories(Storage INTERFACE ${SOLA_SOURCE_DIR}/src)
target_link_libraries(Storage INTERFACE minhton_core_definitions)
