# Algorithms sim
add_library(minhton_algorithms_sim STATIC)
target_sources(minhton_algorithms_sim
  PRIVATE
    ${MINHTON_SOURCE_DIR}/src/algorithms/join/join_algorithm_general.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/join/join_algorithm_general_helper.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/join/minhton_join_algorithm.cpp

    ${MINHTON_SOURCE_DIR}/src/algorithms/leave/leave_algorithm_general.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/leave/leave_algorithm_general_helper.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/leave/minhton_leave_algorithm.cpp

    ${MINHTON_SOURCE_DIR}/src/algorithms/search_exact/search_exact_algorithm_general.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/search_exact/minhton_search_exact_algorithm.cpp

    ${MINHTON_SOURCE_DIR}/src/algorithms/misc/bootstrap_algorithm_general.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/misc/response_algorithm_general.cpp

    ${MINHTON_SOURCE_DIR}/src/algorithms/find_end/minhton_find_end_algorithm_helper.cpp

    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/local_data.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/distributed_data.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/find_query.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/find_query_parser.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/dsn_handler.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/node_data.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/minhton_entity_search_algorithm.cpp
    ${MINHTON_SOURCE_DIR}/src/algorithms/esearch/minhton_entity_search_algorithm_helper.cpp
)

target_include_directories(minhton_algorithms_sim 
    PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_algorithms_sim 
  PUBLIC
    minhton_core_network_info
    minhton_core_node_info_sim
    minhton_core_routing_table_sim
    minhton_message_serialize
    minhton_message_sim
    minhton_core_definitions
    minhton_core_access_container_sim
    peglib
  PRIVATE
    minhton_core_routing_calculations
    minhton_logging
    minhton_utils_uuid_sim
    minhton_exception_algorithm
)
