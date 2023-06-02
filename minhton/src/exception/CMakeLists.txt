# Algorithm
add_library(minhton_exception_algorithm STATIC algorithm_exception.cpp)
target_include_directories(minhton_exception_algorithm
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)

# FSM
add_library(minhton_exception_fsm STATIC fsm_exception.cpp)
target_include_directories(minhton_exception_fsm
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_exception_fsm PUBLIC minhton_fsm minhton_message)

# Invalid message
add_library(minhton_exception_invalid_message STATIC invalid_message_exception.cpp)
target_include_directories(minhton_exception_invalid_message
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_exception_invalid_message PUBLIC minhton_message)
