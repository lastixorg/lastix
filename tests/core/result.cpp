#include "catch2/catch_test_macros.hpp"
#include "lastix/core/result.hpp"
#include "lastix/core/number.hpp"

#include <string>

using namespace lx::core;

enum class ErrorA {
    IoError,
    PermissionDenied,
    NotFound,
};

enum class ErrorB {
    FileError,
    SomeOtherError,
};

template <> struct lx::trait::FromImpl<ErrorA, ErrorB> {
        static auto from(ErrorA) -> ErrorB {
            return ErrorB::FileError;
        }
};

TEST_CASE("Result basic Ok construction", "[lx::core::Result]") {
    Result<i32, void> r = Ok(42);
    REQUIRE(r.is_ok());
    REQUIRE(!r.is_err());
    REQUIRE(r.unwrap() == 42);
    REQUIRE(r.ok().unwrap() == 42);
    REQUIRE(r.err() == None);
}

TEST_CASE("Result basic Err construction", "[lx::core::Result]") {
    Result<void, std::string> r = Err("failure");
    REQUIRE(!r.is_ok());
    REQUIRE(r.is_err());
    REQUIRE(r.unwrap_err() == "failure");
    REQUIRE(r.err().unwrap() == "failure");
    REQUIRE(r.ok() == None);
}

TEST_CASE("Result void Ok", "[lx::core::Result]") {
    Result<void, std::string> r = Ok();
    REQUIRE(r.is_ok());
    REQUIRE(!r.is_err());
}

TEST_CASE("Result copy", "[lx::core::Result]") {
    Result<i32, std::string> r0 = Ok(10);
    Result<i32, std::string> r1 = Err("failure");

    auto c0 = r0;
    auto c1 = r1;

    REQUIRE(c0.is_ok());
    REQUIRE(c1.is_err());
    REQUIRE(c0 == r0);
    REQUIRE(c1 == r1);
    REQUIRE(c0.unwrap() == 10);
    REQUIRE(c1.unwrap_err() == "failure");
}

TEST_CASE("Result move", "[lx::core::Result]") {
    Result<i32, std::string> r0 = Ok(10);
    auto c0 = std::move(r0);
    REQUIRE(c0.is_ok());
    REQUIRE(c0.unwrap() == 10);
}

TEST_CASE("Result construction with From trait", "[lx::core::Result]") {
    auto f0 = [] -> Result<void, ErrorB> {
        auto f1 = [] -> Result<void, ErrorA> {
            return Err(ErrorA::IoError);
        };

        return Err(f1().unwrap_err());
    };

    auto r0 = f0();

    REQUIRE(r0.is_err());
    REQUIRE(r0.unwrap_err() == ErrorB::FileError);
}
