# Object library without UUID generator
add_library(natter_utils_obj OBJECT
  utils.cpp
  utils.h
  tree_helper.cpp
  tree_helper.h
  ${natter_SOURCE_DIR}/include/natter/minhcast_level_number.h
)
target_include_directories(natter_utils_obj
        PUBLIC
        ${natter_SOURCE_DIR}/include
        ${natter_SOURCE_DIR}/src
)
target_link_libraries(natter_utils_obj PUBLIC ${LIBUUID})

add_library(natter_utils STATIC)
target_sources(natter_utils
  PRIVATE
  $<TARGET_OBJECTS:natter_utils_obj>
  ${natter_SOURCE_DIR}/include/natter/uuid.h
  uuid_generator.cpp
)
target_link_libraries(natter_utils PUBLIC ${LIBUUID})
target_include_directories(natter_utils
        PUBLIC
        ${natter_SOURCE_DIR}/include
        ${natter_SOURCE_DIR}/src
)
