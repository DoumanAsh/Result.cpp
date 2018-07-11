include(ExternalProject)
ExternalProject_Add(
    catch
    PREFIX "catch"
    DOWNLOAD_NO_EXTRACT 1
    URL https://github.com/catchorg/Catch2/releases/download/v2.2.2/catch.hpp
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(catch download_dir)
set(catch_dir ${download_dir})

file(GLOB_RECURSE test_SRC "*.cpp")
add_executable(utest ${test_SRC})
add_dependencies(utest catch)
target_link_libraries(utest result)
target_include_directories(utest PUBLIC ${catch_dir})

add_test(NAME result COMMAND utest)
