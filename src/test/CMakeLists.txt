

add_executable(test_sequence test_sequence.cpp)
add_test(NAME SEQUENSE_TEST COMMAND test_sequence 50 80)
set_property(TEST SEQUENSE_TEST PROPERTY PASS_REGULAR_EXPRESSION "50 130 210 50 130 210 50 130 210 50"  )