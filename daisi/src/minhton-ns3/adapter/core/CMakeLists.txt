# MINHTON node sim
add_library(minhton_core_node_sim STATIC)
target_sources(minhton_core_node_sim
  PRIVATE
  ${MINHTON_SOURCE_DIR}/src/core/node.cpp
  ${MINHTON_SOURCE_DIR}/src/core/minhton.cpp
)
target_include_directories(minhton_core_node_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_node_sim
    PUBLIC
        minhton_core_connection_info_sim
        minhton_algorithms_sim
        minhton_core_definitions
        minhton_core_logic_container_sim
        minhton_core_node_info_sim
        minhton_core_routing_table_sim
        minhton_core_watchdog_sim
        minhton_logging
        minhton_message_sim
        minhton_network_facade_sim
        minhton_utils_config_reader
        minhton_fsm_sim
        minhton_utils_procedure_info_sim
    PRIVATE
        minhton_core_access_container_sim
        minhton_exception_fsm_sim
        minhton_exception_invalid_message_sim
        solanet_uuid_generator_sim
        minhton_utils_serializer_cereal_sim
        minhton_timestamp_helper_sim
)

# Watchdog for simulation
add_library(minhton_core_watchdog_sim STATIC minhton_watchdog_ns3.cpp ${MINHTON_SOURCE_DIR}/include/minhton/core/watchdog.h)
target_include_directories(minhton_core_watchdog_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include/minhton # TODO Should only be include/
)
target_link_libraries(minhton_core_watchdog_sim
    PUBLIC
        minhton_core_constants
    PRIVATE
        ns3::libcore
)

# Logic Container sim
add_library(minhton_core_logic_container_sim STATIC ${MINHTON_SOURCE_DIR}/src/core/logic_container.cpp)
target_include_directories(minhton_core_logic_container_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_logic_container_sim
    PUBLIC
        minhton_algorithms_sim
        minhton_core_access_container_sim
        minhton_core_constants
        minhton_core_definitions
        minhton_message_sim
        minhton_utils_algorithm_types_container
)

# Access container sim
add_library(minhton_core_access_container_sim INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/core/access_container.h)
target_include_directories(minhton_core_access_container_sim
    INTERFACE
      ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_access_container_sim
    INTERFACE
      minhton_core_node_info_sim
      minhton_core_routing_table_sim
      minhton_message_sim
      minhton_utils_procedure_info_sim
      minhton_utils_timeout_lengths_container
      minhton_fsm_sim
)

# Node Info
add_library(minhton_core_node_info_sim STATIC ${MINHTON_SOURCE_DIR}/src/core/node_info.cpp)
target_include_directories(minhton_core_node_info_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_node_info_sim
  PUBLIC
    minhton_core_physical_node_info
    minhton_core_logical_node_info_sim
    solanet_serialize
)

# Logical node info
add_library(minhton_core_logical_node_info_sim STATIC ${MINHTON_SOURCE_DIR}/src/core/logical_node_info.cpp)
target_include_directories(minhton_core_logical_node_info_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_logical_node_info_sim
  PUBLIC
    solanet_serialize
    solanet_uuid
    solanet_uuid_generator_sim
  PRIVATE
    minhton_core_constants
    minhton_core_routing_calculations
)

# Routing Table
add_library(minhton_core_routing_table_sim STATIC 
    ${MINHTON_SOURCE_DIR}/src/core/routing_information_general_helper.cpp 
    ${MINHTON_SOURCE_DIR}/src/core/routing_information_table.cpp 
    ${MINHTON_SOURCE_DIR}/src/core/routing_information.cpp)
target_include_directories(minhton_core_routing_table_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_routing_table_sim
    PUBLIC
      minhton_core_constants
      minhton_core_node_info_sim
      minhton_core_routing_calculations
      minhton_logging
      # minhton_exception_fsm
      # minhton_exception_invalid_message
      # minhton_exception_algorithm
)

# Connection info
add_library(minhton_core_connection_info_sim INTERFACE ${MINHTON_SOURCE_DIR}/include/minhton/core/connection_info.h)
target_include_directories(minhton_core_connection_info_sim
    INTERFACE
      ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_core_connection_info_sim
    INTERFACE
      minhton_core_constants
      minhton_core_node_info_sim
)
