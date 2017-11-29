
#include <vector>
#include <iostream>
#include <cassert>
#include <string>

#include <result.hpp>

void try1() {
    const auto ok = result::Result<int, std::string>::ok(1);
    const auto error = result::Result<int, std::string>::error("lolka");

    assert(ok.is_ok());
    assert(!ok.is_err());

    assert(!error.is_ok());
    assert(error.is_err());

    const auto ok_value = ok.unwrap();
    const auto error_value = error.unwrap_err();

    const auto ok_other_value = ok.unwrap_or(3);
    const auto other_value = error.unwrap_or(2);

    assert(ok_value == 1);
    assert(error_value == "lolka");
    assert(other_value == 2);
    assert(ok_other_value == 1);

}

void try2() {
    typedef std::vector<int> Vec;

    const auto ok = result::Result<Vec, std::string>::ok(Vec({1, 2, 3, 4, 5}));

    const auto ok_value = ok.unwrap();
    const auto ok_other_value = ok.unwrap_or(Vec(1, 5));

    assert(ok_value == Vec({1, 2, 3, 4, 5}));
    assert(ok_other_value == Vec({1, 2, 3, 4, 5}));
}

decltype(auto) return_pod_res_with_move() {
    auto ok = result::Result<int, bool>::ok(1);
    return ok;
}

decltype(auto) return_non_pod_res_with_move() {
    auto ok = result::Result<std::vector<int>, std::string>::ok(1, 2);
    return ok;
}

int main() {
    try1();
    try2();
    auto pod_moved = return_pod_res_with_move();
    assert(pod_moved.is_ok());
    assert(pod_moved.unwrap() == 1);
    auto non_pod_moved = return_non_pod_res_with_move();
    assert(non_pod_moved.is_ok());
    assert(non_pod_moved.unwrap() == std::vector<int>(1, 2));

    std::cout << "All ok, onii-chan!\n";
}
