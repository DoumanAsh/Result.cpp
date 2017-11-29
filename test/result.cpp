#include <result.hpp>

#include <iostream>
#include <cassert>
#include <string>

int main() {
    //const auto result = result::Result<int, std::string>::error(std::string("lolka"));
    const auto ok = result::Result<int, std::string>::ok(1);
    const auto error = result::Result<int, std::string>::error("lolka");

    const auto ok_value = ok.unwrap();
    const auto error_value = error.unwrap_err();

    std::cout << "Ok=" << ok_value << "\n"
              << "Err=" << error_value << "\n";
    assert(ok_value == 1);
}
