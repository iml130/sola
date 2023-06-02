add_library(natter_logging STATIC)
target_sources(natter_logging
  PRIVATE
    logger.cpp
    logger.h
)
target_link_libraries(natter_logging natterLoggerInterface)
