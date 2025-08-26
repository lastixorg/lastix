#pragma once

#include "lastix/core/diagnostics.hpp"

#include <optional>

namespace lx::core {

    using NoneType = std::nullopt_t;
    static constexpr auto None = std::nullopt;

    template <class T> class Some {

        public:
            explicit Some(T value) noexcept : _value(std::move(value)) {
            }

            // template <class... Args>
            // explicit Some(Args&&... args) noexcept
            //     : _value(std::forward<Args>(args)...) {
            // }

            auto operator*() & noexcept -> T& {
                return _value;
            }

            auto operator*() const& noexcept -> const T& {
                return _value;
            }

            auto operator*() && noexcept -> T&& {
                return std::move(_value);
            }

        private:
            T _value;
    };

    template <class U> Some(Some<U> s) -> Some<Some<U>>;

    template <class T> class Option {

        public:
            Option(NoneType) : _value(None) {
            }

            Option(Some<T> some) : _value(*std::move(some)) {
            }

            template <class U>
            requires std::convertible_to<U, T>
            Option(Some<U> some) : _value(*std::move(some)) {
            }

            [[nodiscard]] auto is_some() const noexcept -> bool {
                return _value.has_value();
            }

            [[nodiscard]] auto is_none() const noexcept -> bool {
                return !this->is_some();
            }

            auto unwrap() & noexcept -> T& {
                if (!_value) [[unlikely]]
                    panic("Called unwrap() on None");

                return *_value;
            }

            auto unwrap() const& noexcept -> const T& {
                if (!_value) [[unlikely]]
                    panic("Called unwrap() on None");

                return *_value;
            }

            auto unwrap() && noexcept -> T&& {
                if (!_value) [[unlikely]]
                    panic("Called unwrap() on None");

                return std::move(*_value);
            }

            auto expect(std::string_view msg) & noexcept -> T& {
                if (!_value) [[unlikely]]
                    panic(msg);

                return *_value;
            }

            auto expect(std::string_view msg) const& noexcept -> const T& {
                if (!_value) [[unlikely]]
                    panic(msg);

                return *_value;
            }

            auto expect(std::string_view msg) && noexcept -> T&& {
                if (!_value) [[unlikely]]
                    panic(msg);

                return std::move(*_value);
            }

            auto swap(Option& other) noexcept -> void {
                std::swap(_value, other._value);
            }

            operator bool() const noexcept {
                return this->is_some();
            }

        private:
            std::optional<T> _value;
    };

}; // namespace lx::core
