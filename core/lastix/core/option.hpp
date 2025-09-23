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

            template <class Self>
            auto operator*(this Self&& self) noexcept -> decltype(auto) {
                return std::forward_like<Self>(self._value);
            }

        private:
            T _value;
    };

    template <class U> Some(Some<U> s) -> Some<Some<U>>;

    template <class T> class [[nodiscard]] Option {

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

            template <class Self>
            auto unwrap(this Self&& self,
                        std::source_location loc =
                            std::source_location::current()) noexcept
                -> decltype(auto) {
                return std::forward<Self>(self).expect(
                    "Called unwrap() on None", loc);
            }

            template <class Self>
            auto expect(this Self&& self, std::string_view msg,
                        std::source_location loc =
                            std::source_location::current()) noexcept
                -> decltype(auto) {
                if (!self._value) [[unlikely]]
                    panic(msg, loc);

                return std::forward_like<Self>(*self._value);
            }

            auto swap(Option& other) noexcept -> void {
                std::swap(_value, other._value);
            }

            auto operator==(const Option& other) const noexcept -> bool {
                return _value == other._value;
            }

            auto operator==(NoneType) const noexcept -> bool {
                return this->is_none();
            }

            operator bool() const noexcept {
                return this->is_some();
            }

        private:
            std::optional<T> _value;
    };

}; // namespace lx::core
