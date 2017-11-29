# Set flags if compiler accepts GCC flags
macro(gcc_set_flags_if)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ARGV0}")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARGV0}")
    endif()
endmacro(gcc_set_flags_if)
