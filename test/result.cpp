#include <catch.hpp>

#include <vector>
#include <iostream>
#include <cassert>
#include <string>

#include <result.hpp>

TEST_CASE("try is_result") {
    static_assert(!result::is_result<int>::value);
    static_assert(result::is_result<result::Result<int, std::string>>::value);
    REQUIRE(result::is_result<int>::value == false);
    REQUIRE(result::is_result<result::Result<int, std::string>>::value == true);
}

TEST_CASE("try unwraps") {
    auto ok = result::Result<int, std::string>::ok(1);
    auto error = result::Result<int, std::string>::error("lolka");
    const auto const_ok = result::Result<int, std::string>::ok(1);
    const auto const_error = result::Result<int, std::string>::error("lolka");

    auto ok_value = ok.unwrap();
    auto const_ok_value = const_ok.unwrap();
    auto error_err = error.unwrap_err();
    auto const_error_err = const_error.unwrap_err();

    REQUIRE(ok_value == 1);
    REQUIRE(const_ok_value == 1);
    REQUIRE(error_err == "lolka");
    REQUIRE(const_error_err == "lolka");

    auto moved_ok = std::move(ok).unwrap();
    auto moved_err = std::move(error).unwrap_err();

    REQUIRE(moved_ok == 1);
    REQUIRE(moved_err == "lolka");

    ok = result::Result<int, std::string>::ok(1);
    error = result::Result<int, std::string>::error("lolka");

    ok_value = ok.unwrap();
    error_err = error.unwrap_err();

    REQUIRE(ok_value == 1);
    REQUIRE(error_err == "lolka");
}

TEST_CASE("try unwrap_or") {
    auto ok = result::Result<int, std::string>::ok(1);
    auto error = result::Result<int, std::string>::error("lolka");
    const auto const_ok = result::Result<int, std::string>::ok(1);
    const auto const_error = result::Result<int, std::string>::error("lolka");

    auto ok_value = ok.unwrap_or(2);
    auto const_ok_value = const_ok.unwrap_or(2);
    auto error_err = error.unwrap_or(0);
    auto const_error_err = const_error.unwrap_or(0);

    REQUIRE(ok_value == 1);
    REQUIRE(const_ok_value == 1);
    REQUIRE(error_err == 0);
    REQUIRE(const_error_err == 0);

    auto moved_ok = std::move(ok).unwrap_or(2);
    auto moved_err = std::move(error).unwrap_or(0);

    REQUIRE(moved_ok == 1);
    REQUIRE(moved_err == 0);
}

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

TEST_CASE("try map pod type from return") {
    auto lambda = [](int value) {
        REQUIRE(value == 1);
        return 2;
    };

    auto lambda_err = [](bool value) {
        REQUIRE(value == false);
        return 0;
    };

    auto map_good = return_pod_res().map(lambda);
    REQUIRE(map_good.is_ok());
    REQUIRE(map_good.unwrap() == 2);
    REQUIRE(map_good.value() != nullptr);
    REQUIRE(*(map_good.value()) == 2);
    REQUIRE(map_good.error() == nullptr);

    auto new_map_good = map_good.map_err(lambda_err);
    REQUIRE(new_map_good.is_ok());
    REQUIRE(new_map_good.unwrap() == 2);
    REQUIRE(new_map_good.value() != nullptr);
    REQUIRE(*(new_map_good.value()) == 2);
    REQUIRE(new_map_good.error() == nullptr);

    auto map_error = decltype(return_pod_res())::error(false).map(lambda);
    REQUIRE(map_error.is_err());
    REQUIRE(map_error.value() == nullptr);
    REQUIRE(map_error.error() != nullptr);
    REQUIRE(map_error.unwrap_err() == false);
    REQUIRE(*(map_error.error()) == false);

    auto new_map_error = map_error.map_err(lambda_err);
    REQUIRE(map_error.is_err());
    REQUIRE(map_error.value() == nullptr);
    REQUIRE(map_error.error() != nullptr);
    REQUIRE(map_error.unwrap_err() == 0);
    REQUIRE(*(map_error.error()) == 0);
}

TEST_CASE("try map non-pod type from return") {
    decltype(auto) lambda = [](std::vector<int> value) {
        REQUIRE(value == *return_non_pod_res().value());
        return std::vector({3, 4, 5});
    };

    decltype(auto) lambda_err = [](std::string value) {
        REQUIRE(value == "test");
        return std::vector({'1', '2'});
    };

    auto map_good = return_non_pod_res().map(lambda);
    REQUIRE(map_good.is_ok());
    REQUIRE(map_good.unwrap() == std::vector({3, 4, 5}));
    REQUIRE(map_good.value() != nullptr);
    REQUIRE(*(map_good.value()) == std::vector({3, 4, 5}));
    REQUIRE(map_good.error() == nullptr);

    auto new_map_good = map_good.map_err(lambda_err);
    REQUIRE(new_map_good.is_ok());
    REQUIRE(new_map_good.unwrap() == std::vector({3, 4, 5}));
    REQUIRE(new_map_good.value() != nullptr);
    REQUIRE(*(new_map_good.value()) == std::vector({3, 4, 5}));
    REQUIRE(new_map_good.error() == nullptr);

    auto map_error = decltype(return_non_pod_res())::error("test").map(lambda);
    REQUIRE(map_error.is_err());
    REQUIRE(map_error.value() == nullptr);
    REQUIRE(map_error.error() != nullptr);
    REQUIRE(map_error.unwrap_err() == "test");
    REQUIRE(*(map_error.error()) == "test");

    auto new_map_error = map_error.map_err(lambda_err);
    REQUIRE(new_map_error.is_err());
    REQUIRE(new_map_error.value() == nullptr);
    REQUIRE(new_map_error.error() != nullptr);
    REQUIRE(new_map_error.unwrap_err() == std::vector<char>({'1', '2'}));
    REQUIRE(*(new_map_error.error()) == std::vector<char>({'1', '2'}));
}

TEST_CASE("try helpers Ok/Err") {
    result::Result<int, std::string> res_ok = result::Ok(1);
    result::Result<int, std::string> res_err = result::Err(std::string("lolka"));

    REQUIRE(res_ok.is_ok());
    REQUIRE(res_ok.unwrap() == 1);
    REQUIRE(res_err.is_err());
    REQUIRE(res_err.unwrap_err() == "lolka");
}

TEST_CASE("try and_then") {
    decltype(auto) lambda = [](int value) {
        REQUIRE(value == 1);
        return result::Result<char, std::string>::ok('c');
    };

    result::Result<int, std::string> res_ok = result::Ok(1);
    result::Result<int, std::string> res_err = result::Err(std::string("lolka"));

    auto new_res_ok = res_ok.and_then(lambda);
    auto new_res_err = res_err.and_then(lambda);

    REQUIRE(new_res_ok.is_ok());
    REQUIRE(new_res_ok.unwrap() == 'c');
    REQUIRE(new_res_err.is_err());
    REQUIRE(new_res_err.unwrap_err() == "lolka");
}

TEST_CASE("try or_else") {
    decltype(auto) lambda = [](std::string error) {
        REQUIRE(error == "lolka");
        return result::Result<int, char>::error('c');
    };

    result::Result<int, std::string> res_ok = result::Ok(1);
    result::Result<int, std::string> res_err = result::Err(std::string("lolka"));

    auto new_res_ok = res_ok.or_else(lambda);
    auto new_res_err = res_err.or_else(lambda);

    REQUIRE(new_res_ok.is_ok());
    REQUIRE(new_res_ok.unwrap() == 1);
    REQUIRE(new_res_err.is_err());
    REQUIRE(new_res_err.unwrap_err() == 'c');
}
