
file(GLOB_RECURSE test_SRC "*.cpp")
add_executable(utest ${test_SRC})
target_link_libraries(utest result)
