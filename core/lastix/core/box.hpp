#pragma once

#include "lastix/core/diagnostics.hpp"
#include <utility>

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

    template <class T, class Deleter = DefaultDeleter<T>> class Box {

        public:
            template <class... Args>
            explicit Box(Args &&...args) noexcept
                : _ptr(new T(std::forward<Args>(args)...)) {
            }

            template <class U>
            requires std::derived_from<U, T>
            Box(Box<U> &&other) noexcept : _ptr(other.release()) {
            }

            ~Box() noexcept {
                this->reset();
            }

            Box(Box &&box) noexcept : _ptr(box.release()) {
            }

            [[nodiscard]] static auto unsafe_from_raw(T *ptr)
                -> Box<T, Deleter> {

                return Box<T, Deleter>(ptr);
            }

            template <class U>
            requires std::derived_from<U, T>
            auto operator=(Box<U> &&other) noexcept -> Box & {

                this->reset();
                _ptr = other.release();

                return *this;
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

                Deleter{}(std::exchange(_ptr, nullptr));
            }

            [[nodiscard]] auto release() noexcept -> T * {

                return std::exchange(_ptr, nullptr);
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
            Box(T *ptr) : _ptr(ptr) {
            }

        private:
            T *_ptr = nullptr;
    };

}; // namespace lx::core
