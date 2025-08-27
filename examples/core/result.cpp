#include "lastix/core/result.hpp"
#include "lastix/core/number.hpp"

#include <string>
#include <print>

using namespace lx::core;

// Result<T, E> currently does not support T or E being void
struct Empty {};

static auto foo() -> Result<i32, std::string> {

    return Ok(-100);
}

static auto bar() -> Result<i32, std::string> {

    auto x = foo().expect("Ok() is expected");

    if (x > 0) return Ok(x);
    else return Err("x > 0 is expected");
}

static auto baz() -> Result<Empty, i32> {

    auto val = bar();

    if (val.is_ok()) {
        std::println("bar() succeeded: {}", val.ok().unwrap());
        return Ok(Empty());
    }

    auto err = val.unwrap_err();
    std::println("bar() failed: {}", err);

    return Err(-1);
}

auto main() -> i32 {

    auto res = baz();

    return res.is_ok() ? 0 : res.unwrap_err();
}
