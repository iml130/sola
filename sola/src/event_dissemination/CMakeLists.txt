add_library(EventDissemination INTERFACE)
target_include_directories(EventDissemination INTERFACE ${SOLA_SOURCE_DIR}/src)

add_subdirectory(natter)
