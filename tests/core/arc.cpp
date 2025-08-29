#include "catch2/catch_test_macros.hpp"
#include "lastix/core/arc.hpp"
#include "memory_helpers.hpp"

TEST_CASE("Arc basic construction", "[lx::core::Arc]") {
    auto ptr = Arc<TestStruct>(42);
    REQUIRE(static_cast<bool>(ptr));
    REQUIRE(ptr->x == 42);
    REQUIRE((*ptr).x == 42);

    auto sc = ptr.strong_count();
    auto wc = ptr.weak_count();

    REQUIRE(sc != None);
    REQUIRE(wc != None);
    REQUIRE(sc.unwrap() == 1);
    REQUIRE(wc.unwrap() == 0);
}

TEST_CASE("Arc unsafe_from_raw", "[lx::core::Arc]") {
    auto raw = new TestStruct(5);
    auto ptr = Arc<TestStruct>::unsafe_from_raw(raw);
    REQUIRE(static_cast<bool>(ptr));
    REQUIRE(ptr->x == 5);
    REQUIRE(ptr.unsafe_get() == raw);
}

TEST_CASE("Arc swap", "[lx::core::Arc]") {
    auto a = Arc<TestStruct>(5);
    auto b = Arc<TestStruct>(10);
    a.swap(b);
    REQUIRE(a->x == 10);
    REQUIRE(b->x == 5);
}

TEST_CASE("Arc custom deleter", "[lx::core::Arc]") {

    FlagDeleter::deleted = false;
    {
        auto ptr = Arc<i32, FlagDeleter>(320);
        REQUIRE(*ptr == 320);
    }
    REQUIRE(FlagDeleter::deleted);
}

TEST_CASE("Arc reset", "[lx::core::Arc]") {

    FlagDeleter::deleted = false;
    {
        auto ptr = Arc<i32, FlagDeleter>(52);
        REQUIRE(*ptr == 52);
        ptr.reset();
        REQUIRE(ptr.strong_count() == None);
        REQUIRE(ptr.weak_count() == None);
        REQUIRE(!static_cast<bool>(ptr));

        // Try to reset null Arc
        ptr.reset();
    }
    REQUIRE(FlagDeleter::deleted);
}

TEST_CASE("Arc move from derived", "[lx::core::Arc]") {
    auto a = Arc<Derived>();
    auto b = Arc<Base>(std::move(a));
    REQUIRE(!static_cast<bool>(a));
    REQUIRE(b->a == 0);
}

TEST_CASE("Arc copy from derived", "[lx::core::Arc]") {
    auto a = Arc<Derived>();
    auto b = Arc<Base>(a);
    REQUIRE(static_cast<bool>(a));
    REQUIRE(static_cast<bool>(b));
    REQUIRE(a.strong_count().unwrap() == 2);
    REQUIRE(b.strong_count().unwrap() == 2);
    REQUIRE(a->a == 0);
    REQUIRE(b->a == 0);
}

TEST_CASE("Arc copy", "[lx::core::Arc]") {

    auto a = Arc<TestStruct>(123);
    REQUIRE(a.strong_count().unwrap() == 1);
    REQUIRE(a.weak_count().unwrap() == 0);
    {
        auto b = a;
        REQUIRE(b->x == 123);
        REQUIRE(a.strong_count().unwrap() == 2);
        REQUIRE(a.weak_count().unwrap() == 0);
    }
    REQUIRE(a.strong_count().unwrap() == 1);
}

TEST_CASE("Arc mutable dereference", "[lx::core::Arc]") {
    // This should not compile:
    // auto baz = Arc<i32>(42);
    // *baz = 100;

    auto x = Arc<std::atomic<i32>>(42);
    *x = 10;
    REQUIRE(x->load() == 10);
}
