# Node Info
add_library(minhton_core_node_info STATIC node_info.cpp)
target_include_directories(minhton_core_node_info
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_node_info
  PUBLIC
    minhton_core_physical_node_info
    minhton_core_logical_node_info
    minhton_message_serialize
)

# LogicalNodeInfo
add_library(minhton_core_logical_node_info STATIC logical_node_info.cpp)
target_include_directories(minhton_core_logical_node_info
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_logical_node_info
  PUBLIC
    minhton_message_serialize
    minhton_utils_uuid
  PRIVATE
    minhton_core_constants
    minhton_core_routing_calculations
)

# PhysicalNodeInfo
add_library(minhton_core_physical_node_info STATIC physical_node_info.cpp)
target_include_directories(minhton_core_physical_node_info
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_physical_node_info
  PUBLIC
    minhton_message_serialize
)

# Routing Calculations
add_library(minhton_core_routing_calculations STATIC routing_calculations.cpp)
target_include_directories(minhton_core_routing_calculations
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_routing_calculations
  PRIVATE
    minhton_core_constants
)

# Constants
add_library(minhton_core_constants INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/core/constants.h)
target_include_directories(minhton_core_constants
        INTERFACE
        ${MINHTON_SOURCE_DIR}/include
)

# Definitions
add_library(minhton_core_definitions INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/core/definitions.h)
target_include_directories(minhton_core_definitions
        INTERFACE
        ${MINHTON_SOURCE_DIR}/include
)

# Routing Table
add_library(minhton_core_routing_table STATIC routing_information_general_helper.cpp routing_information_table.cpp routing_information.cpp)
target_include_directories(minhton_core_routing_table
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_routing_table
    PUBLIC
      minhton_core_constants
      minhton_core_node_info
      minhton_core_routing_calculations
      minhton_logging
      # minhton_exception_fsm
      # minhton_exception_invalid_message
      # minhton_exception_algorithm
)

# Logic Container
add_library(minhton_core_logic_container STATIC logic_container.cpp)
target_include_directories(minhton_core_logic_container
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_logic_container
    PUBLIC
        minhton_algorithms
        minhton_core_access_container
        minhton_core_constants
        minhton_core_definitions
        minhton_message
        minhton_utils_algorithm_types_container
)

# Access container
add_library(minhton_core_access_container INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/core/access_container.h)
target_include_directories(minhton_core_access_container
    INTERFACE
      ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_access_container
    INTERFACE
      minhton_core_node_info
      minhton_core_routing_table
      minhton_message
      minhton_utils_procedure_info
      minhton_utils_timeout_lengths_container
      minhton_fsm
)

# Watchdog
add_library(minhton_core_watchdog STATIC watchdog.cpp)
target_include_directories(minhton_core_watchdog
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_watchdog
    PUBLIC
        minhton_core_constants
    PRIVATE
        Threads::Threads
        ${LIBEVENT}
        ${LIBEVENT_PTHREADS}
)

# Connection info
add_library(minhton_core_connection_info INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/core/connection_info.h)
target_include_directories(minhton_core_connection_info
    INTERFACE
      ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_connection_info
    INTERFACE
      minhton_core_constants
      minhton_core_node_info
)

# MINHTON node
add_library(minhton_core_node STATIC node.cpp minhton.cpp)
target_include_directories(minhton_core_node
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_node
    PUBLIC
        minhton_core_connection_info
        minhton_algorithms
        minhton_core_definitions
        minhton_core_logic_container
        minhton_core_node_info
        minhton_core_routing_table
        minhton_core_watchdog
        minhton_logging
        minhton_message
        minhton_network_facade
        minhton_utils_config_reader
        minhton_fsm
        minhton_utils_procedure_info
    PRIVATE
        minhton_core_access_container
        minhton_exception_fsm
        minhton_exception_invalid_message
        minhton_utils_uuid
        minhton_utils_serializer_cereal
        minhton_timestamp_helper
)
