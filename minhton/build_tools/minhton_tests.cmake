macro(add_minhton_test)
    set(one_value TEST SOURCE)
    set(multi_value LINKING)
    cmake_parse_arguments(MINHTON_TEST "" "${one_value}" "${multi_value}" ${ARGN})
    
    list(APPEND minhton_test_list ${MINHTON_TEST_TEST})
    set(MINHTON_TEST_${MINHTON_TEST_TEST}_SOURCE ${MINHTON_TEST_SOURCE})
    set(MINHTON_TEST_${MINHTON_TEST_TEST}_LINKING ${MINHTON_TEST_LINKING} Catch2::Catch2WithMain)
endmacro()
