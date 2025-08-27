#include "lastix/core/result.hpp"
#include "lastix/core/error.hpp"
#include "lastix/core/number.hpp"

#include <print>

using namespace lx::core;

static auto foo() -> Result<i32, Error> {

    return Err("File not found");
}

static auto bar() -> Result<i32, Error> {

    return foo().context("Failed to load .env");
}

auto main() -> i32 {

    // Error::write(...) takes sink as argument. It does not require allocations
    // - instead it calls sink as many times as there are error messages
    bar().unwrap_err().write([](auto what) {
        std::println("{}", what);
    });
}
