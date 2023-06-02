add_library(peglib INTERFACE)
target_include_directories(peglib INTERFACE ${MINHTON_SOURCE_DIR}/third_party)
target_link_libraries(peglib INTERFACE Threads::Threads)
