# Looks up boost
macro(lazy_find_boost)
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost 1.64.0 COMPONENTS ${ARGN})

    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    else()
        message(FATAL_ERROR "Cannot find Boost!")
    endif()
endmacro(lazy_find_boost)

macro(set_output_dir dir)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${dir}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${dir}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/bin)
endmacro(set_output_dir)
