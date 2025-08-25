#pragma once

#include "lastix/core/diagnostics.hpp"
#include <utility>

namespace lx::core {

    template <class T> class Box {

        public:
            template <class... Args>
            explicit Box(Args &&...args) noexcept
                : _ptr(new T(std::forward<Args>(args)...)) {
            }

            ~Box() noexcept {
                this->reset();
            }

            Box(Box &&box) noexcept : _ptr(box.release()) {
            }

            auto operator=(Box &&box) noexcept -> Box & {

                if (this != &box) [[likely]] {

                    this->reset();
                    _ptr = box.release();
                }

                return *this;
            }

            [[nodiscard]] auto operator->() noexcept -> T * {

                if (_ptr == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return _ptr;
            }

            [[nodiscard]] auto operator->() const noexcept -> const T * {

                if (_ptr == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return _ptr;
            }

            [[nodiscard]] auto operator*() noexcept -> T & {

                if (_ptr == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return *_ptr;
            }

            [[nodiscard]] auto operator*() const noexcept -> const T & {

                if (_ptr == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return *_ptr;
            }

            explicit operator bool() const noexcept {

                return _ptr != nullptr;
            }

            auto swap(Box &other) noexcept -> void {
                std::swap(_ptr, other._ptr);
            }

            auto reset() noexcept -> void {

                delete std::exchange(_ptr, nullptr);
            }

            [[nodiscard]] auto release() noexcept -> T * {

                T *tmp = _ptr;
                _ptr = nullptr;
                return tmp;
            }

            auto unsafe_get() & noexcept -> T * {

                return _ptr;
            }

            auto unsafe_get() const & noexcept -> const T * {

                return _ptr;
            }

            Box(const Box &) = delete;
            auto operator=(const Box &) -> Box & = delete;

        private:
            T *_ptr = nullptr;
    };

}; // namespace lx::core
