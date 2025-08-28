#include "catch2/catch_test_macros.hpp"
#include "lastix/core/box.hpp"
#include "lastix/core/number.hpp"

using namespace lx::core;

struct TestStruct {
        i32 x = 0;
};

struct Base {
        i32 a = 0;
};

struct Derived : Base {
        i32 b = 7;
};

struct FlagDeleter {
        auto operator()(i32* ptr) const noexcept -> void {

            // Box can call deleter{}(nullptr) if reset() or release() was
            // called. According to c++ standard, it's safe to call delete
            // nullptr, so we are going to do the same
            if (ptr != nullptr) deleted = true;

            delete ptr;
        }

        static thread_local bool deleted;
};

thread_local bool FlagDeleter::deleted = false;

TEST_CASE("Box basic construction", "[lx::core::Box]") {
    auto ptr = Box<TestStruct>(42);
    REQUIRE(ptr->x == 42);
    REQUIRE((*ptr).x == 42);
    REQUIRE(static_cast<bool>(ptr));
}

TEST_CASE("Box unsafe_from_raw", "[lx::core::Box]") {
    auto raw = new TestStruct(5);
    auto b = Box<TestStruct>::unsafe_from_raw(raw);
    REQUIRE(b->x == 5);
    REQUIRE(raw == b.unsafe_get());
}

TEST_CASE("Box move from derived", "[lx::core::Box]") {
    auto a = Box<Derived>();
    auto b = Box<Base>(std::move(a));
    REQUIRE(!static_cast<bool>(a));
    REQUIRE(b->a == 0);
}

TEST_CASE("Box swap", "[lx::core::Box]") {
    auto a = Box<TestStruct>(7);
    auto b = Box<TestStruct>(14);
    a.swap(b);
    REQUIRE(a->x == 14);
    REQUIRE(b->x == 7);
}

TEST_CASE("Box custom deleter", "[lx::core::Box]") {

    FlagDeleter::deleted = false;
    {
        auto ptr = Box<i32, FlagDeleter>(52);
        REQUIRE(*ptr == 52);
    }
    REQUIRE(FlagDeleter::deleted);
}

TEST_CASE("Box reset", "[lx::core::Box]") {

    FlagDeleter::deleted = false;
    {
        auto ptr = Box<i32, FlagDeleter>(52);
        REQUIRE(*ptr == 52);
        ptr.reset();
        REQUIRE(!static_cast<bool>(ptr));
    }
    REQUIRE(FlagDeleter::deleted);
}

TEST_CASE("Box release", "[lx::core::Box]") {

    i32* raw = nullptr;
    FlagDeleter::deleted = false;
    {
        auto ptr = Box<i32, FlagDeleter>(52);
        REQUIRE(*ptr == 52);
        raw = ptr.release();
        REQUIRE(!static_cast<bool>(ptr));
        REQUIRE(*raw == 52);
    }
    REQUIRE(!FlagDeleter::deleted);
    delete raw;
}
