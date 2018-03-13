#include <catch.hpp>

#include <vector>
#include <iostream>
#include <cassert>
#include <string>

#include <result.hpp>

TEST_CASE("try result with pod") {
    const auto ok = result::Result<int, std::string>::ok(1);
    const auto error = result::Result<int, std::string>::error("lolka");

    REQUIRE(ok.is_ok());
    REQUIRE_FALSE(ok.is_err());

    REQUIRE_FALSE(error.is_ok());
    REQUIRE(error.is_err());

    const auto ok_value = ok.unwrap();
    const auto error_value = error.unwrap_err();

    const auto ok_other_value = ok.unwrap_or(3);
    const auto other_value = error.unwrap_or(2);

    REQUIRE(ok.value() != nullptr);
    REQUIRE(*(ok.value()) == 1);
    REQUIRE(ok.error() == nullptr);
    REQUIRE(ok.unwrap() == 1);
    REQUIRE(ok_value == 1);

    REQUIRE(error.error() != nullptr);
    REQUIRE(error.value() == nullptr);
    REQUIRE(*(error.error()) == "lolka");
    REQUIRE(error_value == "lolka");
    REQUIRE(other_value == 2);
    REQUIRE(ok_other_value == 1);
}

TEST_CASE("try result with collection") {
    typedef std::vector<int> Vec;

    const auto ok = result::Result<Vec, std::string>::ok(Vec({1, 2, 3, 4, 5}));
    const auto error = result::Result<Vec, std::string>::error("lolka");

    const auto ok_value = ok.unwrap();
    const auto error_value = error.unwrap_err();

    const auto ok_other_value = ok.unwrap_or(Vec(1, 5));
    const auto error_other_value = error.unwrap_or(Vec({2,5}));

    REQUIRE(ok_value == Vec({1, 2, 3, 4, 5}));
    REQUIRE(ok.value() != nullptr);
    REQUIRE(*(ok.value()) == Vec({1, 2, 3, 4, 5}));
    REQUIRE(ok.error() == nullptr);

    REQUIRE(error_value == "lolka");
    REQUIRE(error.value() == nullptr);
    REQUIRE(error.error() != nullptr);
    REQUIRE(*(error.error()) == "lolka");

    REQUIRE(ok_other_value == Vec({1, 2, 3, 4, 5}));
    REQUIRE(error_other_value == Vec({2, 5}));
}

decltype(auto) return_pod_res() {
    auto ok = result::Result<int, bool>::ok(1);
    return ok;
}

decltype(auto) return_non_pod_res() {
    auto ok = result::Result<std::vector<int>, std::string>::ok(1, 2);
    return ok;
}

TEST_CASE("try return result") {
    auto pod_moved = return_pod_res();
    REQUIRE(pod_moved.is_ok());
    REQUIRE(pod_moved.unwrap() == 1);
    REQUIRE(pod_moved.value() != nullptr);
    REQUIRE(*(pod_moved.value()) == 1);
    REQUIRE(pod_moved.error() == nullptr);
    auto non_pod_moved = return_non_pod_res();
    REQUIRE(non_pod_moved.is_ok());
    REQUIRE(non_pod_moved.unwrap() == std::vector<int>(1, 2));
    REQUIRE(non_pod_moved.value() != nullptr);
    REQUIRE(*(non_pod_moved.value()) == std::vector<int>(1, 2));
    REQUIRE(non_pod_moved.error() == nullptr);
}
