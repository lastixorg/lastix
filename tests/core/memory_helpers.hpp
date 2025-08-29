#pragma once

#include "lastix/core/number.hpp"

using namespace lx::core;

struct TestStruct {
        i32 x = 0;
};

struct Base {

        virtual ~Base() = default;

        i32 a = 0;
};

struct Derived : Base {
        i32 b = 7;
};

struct FlagDeleter {
        auto operator()(i32* ptr) const noexcept -> void {

            // Smart pointer can call deleter{}(nullptr) if reset() or release()
            // was called. According to c++ standard, it's safe to call delete
            // nullptr, so we are going to do the same
            if (ptr != nullptr) deleted = true;

            delete ptr;
        }

        static thread_local bool deleted;
};
