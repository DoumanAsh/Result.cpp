###########################
# Init
##########################
cmake_minimum_required(VERSION 3.5)
set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})
include(LazyUtils)
include(msvc)
include(gcc)

# Generates compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(Result LANGUAGES CXX)

msvc_remove_warning_level()

###########################
# Output dir
##########################
set_output_dir("${CMAKE_BINARY_DIR}/dist")

msvc_set_flags_if("/std:c++latest  /utf-8")

###########################
# Max compile warnings for own code only
##########################
msvc_set_flags_if("/W4")
gcc_set_flags_if("-Wall -Wextra")

###########################
# Lib
##########################
add_subdirectory("lib/")

############
# Tests
############
option(UNIT_TEST "Build unit tests" OFF)
if (UNIT_TEST)
    enable_testing()
    add_subdirectory("test/")
endif()

###########################
# Linter
##########################
option(CLANG_TIDY "Enable Clang tidy checks" OFF)
if (CLANG_TIDY)
    include(ClangTidy)
endif()
