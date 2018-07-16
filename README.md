# Result.cpp

[![Build Status](https://travis-ci.org/DoumanAsh/Result.cpp.svg?branch=master)](https://travis-ci.org/DoumanAsh/Result.cpp)

Rusult type for C++ inspired by Rust.

WIP

## Usage

You can just take header `lib/result.hpp` and place it into your project

```c++
#include <iostream>

#include "result.hpp"

decltype(auto) get_something() {
    //Do some work.

    return result::Result<int, std::string>::ok(1);
}

int main() {
    const auto result = get_something();

    std::cout << "Result=" << result.unwrap_or(0) << "\n";
```
