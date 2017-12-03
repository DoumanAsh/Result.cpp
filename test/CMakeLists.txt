include(ExternalProject)
ExternalProject_Add(
    catch
    PREFIX "catch"
    DOWNLOAD_NO_EXTRACT 1
    URL https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp
    URL_HASH SHA256=6988c82def026a0b862f43ab122bbbd6051b0b7648bfe04c8a150ef62ca5a0a2
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

enable_testing()
add_test(NAME test COMMAND utest)
