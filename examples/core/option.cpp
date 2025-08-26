#include "lastix/core/option.hpp"
#include "lastix/core/number.hpp"

#include <string>
#include <print>

using namespace lx::core;

struct MyStruct {
        i32 x;
        std::string name;

        MyStruct(i32 value, std::string n) : x(value), name(std::move(n)) {
            std::println("MyStruct constructed (x = {}, name = {})", x, name);
        }

        ~MyStruct() {
            std::println("MyStruct destroyed (x = {}, name = {})", x, name);
        }

        auto greet() const -> void {
            std::println("Hello, my name is {} and x = {}", name, x);
        }
};

auto main() -> i32 {
    // Construct Some and None
    Option<MyStruct> o1 = Some(MyStruct{42, "Alice"});
    Option<MyStruct> o2 = None;

    std::println("o1 is_some? {}", o1.is_some());
    std::println("o2 is_none? {}", o2.is_none());

    // Access with unwrap()
    o1.unwrap().greet();

    // Uncommenting the following line will abort the program
    // o2.expect("o2 should not be None");

    // Move unwrap
    Option<std::string> name_opt = Some(std::string("Bob"));
    std::string name = std::move(name_opt).unwrap();
    std::println("Got name: {}", name);

    // Copy construction
    Option<i32> v1 = Some(1337);
    Option<i32> v2 = v1; // copies underlying optional
    std::println("v1.unwrap() = {}, v2.unwrap() = {}", v1.unwrap(),
                 v2.unwrap());

    // Reassignment
    Option<i32> v3 = None;
    v3 = Some(99);
    std::println("v3 after assignment: {}", v3.unwrap());

    // Typical chaining pattern: Option<Option<T>>
    Option<Option<i32>> nested = Some(Some(7));
    std::println("nested.unwrap().unwrap() = {}", nested.unwrap().unwrap());

    // Safe conditional
    Option<i32> maybe = Some(5);
    if (maybe.is_some()) { std::println("maybe holds {}", maybe.unwrap()); }

    return 0;
}
