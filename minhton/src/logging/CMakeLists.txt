add_library(minhton_logging STATIC)

target_sources(minhton_logging
  PRIVATE
    logger.cpp
)

target_include_directories(minhton_logging
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
        )
