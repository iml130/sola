# Object library without UUID generator
add_library(natter_utils STATIC
  tree_helper.cpp
  tree_helper.h
  ${natter_SOURCE_DIR}/include/natter/minhcast_level_number.h
)
target_include_directories(natter_utils
        PUBLIC
        ${natter_SOURCE_DIR}/include
        ${natter_SOURCE_DIR}/src
)
