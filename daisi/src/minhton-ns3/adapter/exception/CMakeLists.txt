# FSM
add_library(minhton_exception_fsm_sim STATIC ${MINHTON_SOURCE_DIR}/src/exception/fsm_exception.cpp)
target_include_directories(minhton_exception_fsm_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_exception_fsm_sim PUBLIC minhton_fsm_sim minhton_message_sim)

# Invalid message
add_library(minhton_exception_invalid_message_sim STATIC ${MINHTON_SOURCE_DIR}/src/exception/invalid_message_exception.cpp)
target_include_directories(minhton_exception_invalid_message_sim
        PUBLIC
        ${MINHTON_SOURCE_DIR}/include
)
target_link_libraries(minhton_exception_invalid_message_sim PUBLIC minhton_message_sim)
