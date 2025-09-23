#pragma once

#include "lastix/core/diagnostics.hpp"
#include "lastix/core/option.hpp"
#include "lastix/trait/from.hpp"

#include <variant>
#include <utility>

namespace lx::core {

    namespace impl {

        template <class T, class Tag> class ResultTaggedValue {
            public:
                ResultTaggedValue() noexcept = default;

                explicit ResultTaggedValue(T value) noexcept
                    : _value(std::move(value)) {
                }

                template <class Self>
                auto operator*(this Self&& self) noexcept -> decltype(auto) {
                    return std::forward_like<Self>(self._value);
                }

                auto operator==(const ResultTaggedValue& other) const noexcept
                    -> bool {
                    return _value == other._value;
                }

            protected:
                T _value;
        };

        template <class Tag> class ResultTaggedValue<void, Tag> {

            public:
                ResultTaggedValue() = default;
        };

        struct OkTag {};
        struct ErrTag {};

        // Handle Ok(Ok(...))
        template <class U>
        ResultTaggedValue(ResultTaggedValue<U, OkTag>)
            -> ResultTaggedValue<ResultTaggedValue<U, OkTag>, OkTag>;

        // Handle Err(Err(...))
        template <class U>
        ResultTaggedValue(ResultTaggedValue<U, ErrTag>)
            -> ResultTaggedValue<ResultTaggedValue<U, ErrTag>, ErrTag>;

        struct Empty {};

        template <class T> struct VoidOrTypeHelper {
                using type = T;
        };

        template <> struct VoidOrTypeHelper<void> {
                using type = Empty;
        };

        template <class T> using VoidOrType = VoidOrTypeHelper<T>::type;

        template <class T>
        concept WithContext = requires(T t) { t.context(""); };

    }; // namespace impl

    using Empty = impl::Empty;

    template <class T = void>
    using Ok = impl::ResultTaggedValue<T, impl::OkTag>;
    template <class E = void>
    using Err = impl::ResultTaggedValue<E, impl::ErrTag>;

    template <class T, class E> class [[nodiscard]] Result {
        public:
            using Value = impl::VoidOrType<T>;
            using Error = impl::VoidOrType<E>;
            using VariantType = std::variant<Value, Error>;

            Result(Ok<Value> value) noexcept : _variant(*std::move(value)) {
            }
            Result(Err<Error> error) noexcept : _variant(*std::move(error)) {
            }

            template <class U = void>
            requires std::same_as<T, void>
            Result(Ok<void>) noexcept : _variant(Value{}) {
            }

            template <class F = void>
            requires std::same_as<E, void>
            Result(Err<void>) noexcept : _variant(Error{}) {
            }

            /// Accept convertible Ok<U> if U -> T
            template <class U>
            requires(!lx::trait::From<U, T> && std::convertible_to<U, Value>)
            Result(Ok<U> value) noexcept : _variant(*std::move(value)) {
            }

            /// Accept convertible Err<F> if F -> E
            template <class F>
            requires(!lx::trait::From<F, E> && std::convertible_to<F, Error>)
            Result(Err<F> error) noexcept : _variant(*std::move(error)) {
            }

            /// Accept From<U, T>
            template <class U>
            requires lx::trait::From<U, T>
            Result(Ok<U> value) noexcept
                : _variant(lx::trait::FromImpl<U, T>::from(*std::move(value))) {
            }

            /// Accept From<F, E>
            template <class F>
            requires lx::trait::From<F, E>
            Result(Err<F> error) noexcept
                : _variant(lx::trait::FromImpl<F, E>::from(*std::move(error))) {
            }

            auto operator==(const Result& other) const noexcept -> bool {
                return _variant == other._variant;
            }

            template <class U = void>
            requires impl::WithContext<Error>
            [[nodiscard]] auto context(std::string_view msg) & noexcept
                -> Result<Value, Error>& {

                if (this->is_err()) {
                    auto& e = this->unwrap_err();
                    e = e.context(msg);
                }

                return *this;
            }

            template <class U = void>
            requires impl::WithContext<Error>
            [[nodiscard]] auto context(std::string_view msg) && noexcept
                -> Result<Value, Error>&& {

                if (this->is_err()) {
                    auto& e = this->unwrap_err();
                    e = e.context(msg);
                }

                return std::move(*this);
            }

            [[nodiscard]] auto is_ok() const noexcept -> bool {
                return std::holds_alternative<Value>(_variant);
            }

            [[nodiscard]] auto is_err() const noexcept -> bool {
                return std::holds_alternative<Error>(_variant);
            }

            [[nodiscard]] auto ok() const& noexcept -> Option<Value> {
                if (auto* p = std::get_if<Value>(&_variant)) return Some(*p);

                return None;
            }

            [[nodiscard]] auto ok() && noexcept -> Option<Value> {
                if (auto* p = std::get_if<Value>(&_variant))
                    return Some(std::move(*p));

                return None;
            }

            [[nodiscard]] auto err() const& noexcept -> Option<Error> {
                if (auto* p = std::get_if<Error>(&_variant)) return Some(*p);

                return None;
            }

            [[nodiscard]] auto err() && noexcept -> Option<Error> {
                if (auto* p = std::get_if<Error>(&_variant))
                    return Some(std::move(*p));

                return None;
            }

            template <class Self>
            auto unwrap(this Self&& self,
                        std::source_location loc =
                            std::source_location::current()) noexcept
                -> decltype(auto) {
                return std::forward<Self>(self).expect("Called unwrap() on Err",
                                                       loc);
            }

            template <class Self>
            auto expect(this Self&& self, std::string_view msg,
                        std::source_location loc =
                            std::source_location::current()) noexcept
                -> decltype(auto) {
                if (auto* p = std::get_if<Value>(&self._variant))
                    return std::forward_like<Self>(*p);

                panic(msg, loc);
            }

            template <class Self>
            auto unwrap_err(this Self&& self,
                            std::source_location loc =
                                std::source_location::current()) noexcept
                -> decltype(auto) {
                return std::forward<Self>(self).expect_err(
                    "Called unwrap_err() on Ok", loc);
            }

            template <class Self>
            auto expect_err(this Self&& self, std::string_view msg,
                            std::source_location loc =
                                std::source_location::current()) noexcept
                -> decltype(auto) {
                if (auto* p = std::get_if<Error>(&self._variant)) {
                    return std::forward_like<Self>(*p);
                };

                panic(msg, loc);
            }

        private:
            VariantType _variant;
    };

}; // namespace lx::core
