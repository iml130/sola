# UUID generator sim
add_library(minhton_utils_uuid_sim STATIC
  ${MINHTON_SOURCE_DIR}/include/minhton/utils/uuid.h
  ${MINHTON_SOURCE_DIR}/src/utils/uuid_tools.cpp
  uuid_generator_sim.cpp
)
target_link_libraries(minhton_utils_uuid_sim 
  PRIVATE
    ${LIBUUID}
    daisi_random_engine
) 
target_include_directories(minhton_utils_uuid_sim
        PUBLIC
          ${MINHTON_SOURCE_DIR}/include
)

# FSM
add_library(minhton_fsm_sim STATIC ${MINHTON_SOURCE_DIR}/src/utils/fsm.cpp)
target_include_directories(minhton_fsm_sim PUBLIC
)
target_link_libraries(minhton_fsm_sim
  PUBLIC
    minhton_core_constants
    minhton_core_node_info_sim
    minhton_message_sim
    fsmlite
)

# Cereal serializer
add_library(minhton_utils_serializer_cereal_sim STATIC
  ${MINHTON_SOURCE_DIR}/src/utils/serializer_cereal.cpp # + .h + serializer.h
)
target_link_libraries(minhton_utils_serializer_cereal_sim
  PUBLIC
    Cereal
    minhton_algorithms_sim
    minhton_core_constants
  PRIVATE
    minhton_message_sim
  ) 
target_include_directories(minhton_utils_serializer_cereal_sim
        PUBLIC
          ${MINHTON_SOURCE_DIR}/include
)

# Procedure info
add_library(minhton_utils_procedure_info_sim STATIC ${MINHTON_SOURCE_DIR}/src/utils/procedure_info.cpp)
target_include_directories(minhton_utils_procedure_info_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_utils_procedure_info_sim
  PUBLIC
    minhton_algorithms_sim
    minhton_core_network_info
    minhton_core_node_info_sim
    minhton_core_logical_node_info_sim
    minhton_core_definitions
  PRIVATE
    minhton_exception_algorithm
)

# Timestamp helper
add_library(minhton_timestamp_helper_sim minhton_timestamp_helper_ns3.cpp ${MINHTON_SOURCE_DIR}/include/minhton/utils/timestamp_helper.h)
target_include_directories(minhton_timestamp_helper_sim PUBLIC ${MINHTON_SOURCE_DIR}/include)
target_link_libraries(minhton_timestamp_helper_sim
                        PRIVATE
                        ns3::libcore
)
