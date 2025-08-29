#include "lastix/core/arc.hpp"

#include <thread>
#include <atomic>
#include <print>

using namespace lx::core;

static auto demo_read_only() -> void {
    // Arc allows shared ownership of a value.
    // Since u16 is not thread-safe (not Sync), it can only be *read*.
    auto ptr = Arc<u16>(42);

    std::println("Thread A: *ptr = {}", *ptr);

    // Trying to modify through Arc<u16> won't compile:
    // *ptr = 100;  // Error: u16 is not Sync

    // Multiple threads may safely *read* the value:
    std::jthread([ptr] {
        std::println("Thread B: *ptr = {}", *ptr);
    });
}

static auto demo_with_atomic() -> void {
    // Arc<std::atomic<T>> is Sync, so concurrent modification is allowed.
    auto ptr = Arc<std::atomic<u16>>(42);

    std::println("Thread A: atomic initial value = {}", ptr->load());

    ptr->store(100);
    std::println("Thread A: atomic after store = {}", ptr->load());

    // Two threads modify the shared atomic concurrently:
    auto t1 = std::jthread([ptr] mutable noexcept {
        ptr->store(54);
    });

    auto t2 = std::jthread([ptr] mutable noexcept {
        ptr->store(123);
    });

    // The final value is whichever store wins last.
    std::println("After concurrent modification, value = {}", ptr->load());
}

auto main() -> i32 {
    std::println("=== Demo: Read-only sharing ===");
    demo_read_only();

    std::println("\n=== Demo: Concurrent modification with atomic ===");
    demo_with_atomic();

    return 0;
}
