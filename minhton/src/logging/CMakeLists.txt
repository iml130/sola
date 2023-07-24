add_library(minhton_logging STATIC)
target_sources(minhton_logging
  PRIVATE
    logger.cpp
)
target_include_directories(minhton_logging
  PUBLIC
  ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_logging
  PUBLIC
  minhton_logger_interface
)

add_library(minhton_logger_interface INTERFACE)
target_sources(minhton_logger_interface
  INTERFACE
    ${MINHTON_SOURCE_DIR}/include/minhton/logging/logger_interface.h
)
target_include_directories(minhton_logger_interface
        INTERFACE
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_logger_interface INTERFACE solanet_uuid)
