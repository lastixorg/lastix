#include "lastix/core/box.hpp"
#include "lastix/core/number.hpp"

#include <string>
#include <print>

using namespace lx::core;

struct MyStruct {

        i32 x;
        std::string name;

        MyStruct(i32 value, std::string n) : x(value), name(std::move(n)) {

            std::println("MyStruct constructed");
        }

        ~MyStruct() {
            std::println("MyStruct destroyed");
        }

        auto greet() const -> void {
            std::println("Hello, my name is {} and x = {}", name, x);
        }
};

auto main() -> i32 {

    // Box uses arguments to construct object in-place
    auto b1 = Box<MyStruct>(42, "BoxedStruct");
    b1->greet();
    std::println("x via b1->x: {}", b1->x);
    std::println("name via *b1: {}", (*b1).name);

    // Move construction
    auto b2 = std::move(b1);
    if (!b1) std::println("b1 is empty after move");
    b2->greet();

    // Move assignment
    auto b3 = Box<MyStruct>(7, "Temp");
    b3 = std::move(b2);
    if (!b2) std::println("b2 is empty after move assignment");
    b3->greet();

    // Release
    MyStruct* raw = b3.release(); // Transfer ownership manually
    if (!b3) std::println("b3 is empty after release");
    raw->greet();
    delete raw;

    // Swap
    auto a = Box<MyStruct>(1, "A");
    auto b = Box<MyStruct>(2, "B");
    std::println("Before swap:");
    a->greet();
    b->greet();

    a.swap(b);
    std::println("After swap:");
    a->greet();
    b->greet();

    // unsafe_get()
    auto c = Box<MyStruct>(52, "Unsafe");
    MyStruct* ptr = c.unsafe_get();
    ptr->greet(); // direct pointer access without nullptr checks. Still managed
                  // by Box<T>

    return 0;
}
