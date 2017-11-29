#include <result.hpp>

#include <vector>
#include <iostream>
#include <cassert>
#include <string>

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

int main() {
    try1();
    try2();

    std::cout << "All ok, onii-chan!\n";
}
