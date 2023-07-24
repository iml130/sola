# General
add_subdirectory(core)
add_subdirectory(utils)

# Protocols
add_subdirectory(minhcast)

if(NATTER_ENABLE_LOGGING)
  add_subdirectory(logging)
  list(APPEND COMPILE_DEFINITIONS "NATTER_ENABLE_LOGGING")
endif()

add_library(natter_interface INTERFACE)
target_sources(natter_interface
  INTERFACE
    ${natter_SOURCE_DIR}/include/natter/natter.h
)

target_include_directories(natter_interface
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(natter_interface
  INTERFACE
    natterMessage
)

add_library(natterMessage INTERFACE)
target_sources(natterMessage
  INTERFACE
    ${natter_SOURCE_DIR}/include/natter/message.h
)
target_include_directories(natterMessage
  INTERFACE
    ${natter_SOURCE_DIR}/include
)
target_link_libraries(natterMessage INTERFACE solanet_uuid)

add_library(natterLoggerInterface INTERFACE)
target_sources(natterLoggerInterface
  INTERFACE
    ${natter_SOURCE_DIR}/include/natter/logger_interface.h
)
target_include_directories(natterLoggerInterface
  INTERFACE
    ${natter_SOURCE_DIR}/include
)
target_link_libraries(natterLoggerInterface
  INTERFACE
  solanet_uuid  
)

#set(PUBLIC_HEADERS
#  ${natter_SOURCE_DIR}/include/natter/natter.h
#)

#set_target_properties(natter PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")


if(COMPILE_DEFINITIONS)
  set_target_properties(natter_interface PROPERTIES COMPILE_DEFINITIONS "${COMPILE_DEFINITIONS}")
endif()
