#pragma once

namespace lx::core {

    template <class T> struct DefaultDeleter {
            auto operator()(T *ptr) const noexcept -> void {
                delete ptr;
            }
    };

    template <class T> struct DefaultDeleter<T[]> {
            auto operator()(T *ptr) const noexcept -> void {
                delete[] ptr;
            }
    };

}; // namespace lx::core
