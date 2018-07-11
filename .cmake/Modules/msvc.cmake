# Set MVSC flags
macro(msvc_set_flags_if)
    if(MSVC)
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ARGV0}")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARGV0}")
    endif()
endmacro(msvc_set_flags_if)

# Removes default warning to avoid redefenition
macro(msvc_remove_warning_level)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endmacro(msvc_remove_warning_level)

