add_library(minhton_algorithms STATIC)
target_sources(minhton_algorithms
  PRIVATE
    join/join_algorithm_general.cpp
    join/join_algorithm_general_helper.cpp
    join/minhton_join_algorithm.cpp

    leave/leave_algorithm_general.cpp
    leave/leave_algorithm_general_helper.cpp
    leave/minhton_leave_algorithm.cpp

    search_exact/search_exact_algorithm_general.cpp
    search_exact/minhton_search_exact_algorithm.cpp

    misc/bootstrap_algorithm_general.cpp
    misc/response_algorithm_general.cpp

    find_end/minhton_find_end_algorithm_helper.cpp

    esearch/local_data.cpp
    esearch/distributed_data.cpp
    esearch/find_query.cpp
    esearch/find_query_parser.cpp
    esearch/dsn_handler.cpp
    esearch/node_data.cpp
    esearch/minhton_entity_search_algorithm.cpp
    esearch/minhton_entity_search_algorithm_helper.cpp
)

target_include_directories(minhton_algorithms PUBLIC
  ${MINHTON_SOURCE_DIR}/include # TODO
)

target_link_libraries(minhton_algorithms
  PUBLIC
    minhton_core_physical_node_info
    minhton_core_node_info
    minhton_core_routing_table
    solanet_serialize
    minhton_message
    minhton_core_definitions
    minhton_core_access_container
    peglib
  PRIVATE
    minhton_core_routing_calculations
    minhton_logging
    solanet_uuid_generator
    minhton_exception_algorithm
)
