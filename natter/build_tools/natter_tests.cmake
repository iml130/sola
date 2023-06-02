macro(add_natter_test)
    set(one_value TEST SOURCE)
    set(multi_value LINKING)
    cmake_parse_arguments(NATTER_TEST "" "${one_value}" "${multi_value}" ${ARGN})
    
    list(APPEND natter_test_list ${NATTER_TEST_TEST})
    set(NATTER_TEST_${NATTER_TEST_TEST}_SOURCE ${NATTER_TEST_SOURCE})
    set(NATTER_TEST_${NATTER_TEST_TEST}_LINKING ${NATTER_TEST_LINKING} Catch2::Catch2WithMain)
endmacro()
