# FSM
add_library(minhton_fsm STATIC fsm.cpp)
target_link_libraries(minhton_fsm
  PUBLIC
    minhton_core_constants
    minhton_core_node_info
    minhton_message
    fsmlite
)

# Algorithm types container
add_library(minhton_utils_algorithm_types_container INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/utils/algorithm_types_container.h)
target_include_directories(minhton_utils_algorithm_types_container
        INTERFACE
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_utils_algorithm_types_container INTERFACE minhton_core_constants)

# Timeout lenghts container
add_library(minhton_utils_timeout_lengths_container INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/utils/timeout_lengths_container.h)
target_include_directories(minhton_utils_timeout_lengths_container
        INTERFACE
        ${MINHTON_SOURCE_DIR}/include
)

# Procedure info
add_library(minhton_utils_procedure_info STATIC procedure_info.cpp)
target_include_directories(minhton_utils_procedure_info
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_utils_procedure_info
  PUBLIC
    minhton_algorithms
    minhton_core_network_info
    minhton_core_node_info
    minhton_core_logical_node_info
    minhton_core_definitions
  PRIVATE
    minhton_exception_algorithm
)

# Procedure info
add_library(minhton_utils_config_reader STATIC config_node.cpp config_reader.cpp)
target_include_directories(minhton_utils_config_reader
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_utils_config_reader
  PUBLIC
    minhton_core_constants
    minhton_utils_timeout_lengths_container
    minhton_utils_algorithm_types_container
  PRIVATE
    yaml-cpp
)

# UUID generator
add_library(minhton_utils_uuid STATIC
  ${MINHTON_SOURCE_DIR}/include/minhton/utils/uuid.h
  uuid_tools.cpp
  uuid_generator.cpp
)
target_link_libraries(minhton_utils_uuid ${LIBUUID}) 
target_include_directories(minhton_utils_uuid
        PUBLIC
          ${MINHTON_SOURCE_DIR}/include
)

# Cereal serializer
add_library(minhton_utils_serializer_cereal STATIC
  serializer_cereal.cpp # + .h + serializer.h
)
target_link_libraries(minhton_utils_serializer_cereal
  PUBLIC  
    Cereal
    minhton_algorithms
    minhton_core_constants
  PRIVATE
    minhton_message
  ) 
target_include_directories(minhton_utils_serializer_cereal
        PUBLIC
          ${MINHTON_SOURCE_DIR}/include
)

add_library(minhton_timestamp_helper timestamp_helper.cpp ${MINHTON_SOURCE_DIR}/include/minhton/utils/timestamp_helper.h)
target_include_directories(minhton_timestamp_helper PUBLIC ${MINHTON_SOURCE_DIR}/include)
