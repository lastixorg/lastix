#pragma once

#include "lastix/core/diagnostics.hpp"
#include "lastix/core/option.hpp"

#include <variant>
#include <utility>

namespace lx::core {

    namespace impl {

        template <class T, class Tag> class ResultEnumeration {
            public:
                ResultEnumeration() noexcept = default;

                explicit ResultEnumeration(T value) noexcept
                    : _value(std::move(value)) {
                }

                auto operator*() & noexcept -> T& {
                    return _value;
                }

                auto operator*() const& noexcept -> const T& {
                    return _value;
                }

                auto operator*() && noexcept -> T&& {
                    return std::move(_value);
                }

                auto operator==(const ResultEnumeration& other) const noexcept
                    -> bool {
                    return _value == other._value;
                }

            protected:
                T _value;
        };

        template <class Tag> class ResultEnumeration<void, Tag> {

            public:
                ResultEnumeration() = default;
        };

        struct OkTag {};
        struct ErrTag {};

        // Handle Ok(Ok(...))
        template <class U>
        ResultEnumeration(ResultEnumeration<U, OkTag>)
            -> ResultEnumeration<ResultEnumeration<U, OkTag>, OkTag>;

        // Handle Err(Err(...))
        template <class U>
        ResultEnumeration(ResultEnumeration<U, ErrTag>)
            -> ResultEnumeration<ResultEnumeration<U, ErrTag>, ErrTag>;

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
    using Ok = impl::ResultEnumeration<T, impl::OkTag>;
    template <class E = void>
    using Err = impl::ResultEnumeration<E, impl::ErrTag>;

    template <class T, class E> class [[nodiscard]] Result {
        public:
            using Value = impl::VoidOrType<T>;
            using Error = impl::VoidOrType<E>;
            using VariantType = std::variant<Ok<Value>, Err<Error>>;

            Result(Ok<Value> value) noexcept : _variant(std::move(value)) {
            }
            Result(Err<Error> error) noexcept : _variant(std::move(error)) {
            }

            template <class U = void>
            requires std::same_as<T, void>
            Result(Ok<void>) noexcept : _variant(Ok<Value>()) {
            }

            template <class F = void>
            requires std::same_as<E, void>
            Result(Err<void>) noexcept : _variant(Err<Error>()) {
            }

            /// Accept convertible Ok<U> if U -> T
            template <class U>
            requires std::convertible_to<U, Value>
            Result(Ok<U> value) noexcept
                : _variant(Ok<Value>{*std::move(value)}) {
            }

            /// Accept convertible Err<F> if F -> E
            template <class F>
            requires std::convertible_to<F, Error>
            Result(Err<F> error) noexcept
                : _variant(Err<Error>{*std::move(error)}) {
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
                return std::holds_alternative<Ok<Value>>(_variant);
            }

            [[nodiscard]] auto is_err() const noexcept -> bool {
                return std::holds_alternative<Err<Error>>(_variant);
            }

            [[nodiscard]] auto ok() const& noexcept -> Option<Value> {
                if (auto* p = std::get_if<Ok<Value>>(&_variant))
                    return Some(**p);

                return None;
            }

            [[nodiscard]] auto ok() && noexcept -> Option<Value> {
                if (auto* p = std::get_if<Ok<Value>>(&_variant))
                    return Some(std::move(**p));

                return None;
            }

            [[nodiscard]] auto err() const& noexcept -> Option<Error> {
                if (auto* p = std::get_if<Err<Error>>(&_variant))
                    return Some(**p);

                return None;
            }

            [[nodiscard]] auto err() && noexcept -> Option<Error> {
                if (auto* p = std::get_if<Err<Error>>(&_variant))
                    return Some(std::move(**p));

                return None;
            }

            auto unwrap() & noexcept -> Value& {
                if (auto* p = std::get_if<Ok<Value>>(&_variant)) return **p;

                panic("Called unwrap() on Err");
            }

            auto unwrap() const& noexcept -> const Value& {
                if (auto* p = std::get_if<Ok<Value>>(&_variant)) return **p;

                panic("Called unwrap() on Err");
            }

            auto unwrap() && noexcept -> Value&& {
                if (auto* p = std::get_if<Ok<Value>>(&_variant))
                    return std::move(**p);

                panic("Called unwrap() on Err");
            }

            auto expect(std::string_view msg) & noexcept -> Value& {
                if (auto* p = std::get_if<Ok<Value>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect(std::string_view msg) const& noexcept -> const Value& {
                if (auto* p = std::get_if<Ok<Value>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect(std::string_view msg) && noexcept -> Value&& {
                if (auto* p = std::get_if<Ok<Value>>(&_variant))
                    return std::move(**p);

                panic(msg);
            }

            auto unwrap_err() & noexcept -> Error& {
                if (auto* p = std::get_if<Err<Error>>(&_variant)) return **p;

                panic("Called unwrap_err() on Ok");
            }

            auto unwrap_err() const& noexcept -> const Error& {
                if (auto* p = std::get_if<Err<Error>>(&_variant)) return **p;

                panic("Called unwrap_err() on Ok");
            }

            auto unwrap_err() && noexcept -> Error&& {
                if (auto* p = std::get_if<Err<Error>>(&_variant))
                    return std::move(**p);

                panic("Called unwrap_err() on Ok");
            }

            auto expect_err(std::string_view msg) & noexcept -> Error& {
                if (auto* p = std::get_if<Err<Error>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect_err(std::string_view msg) const& noexcept
                -> const Error& {
                if (auto* p = std::get_if<Err<Error>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect_err(std::string_view msg) && noexcept -> Error&& {
                if (auto* p = std::get_if<Err<Error>>(&_variant))
                    return std::move(**p);

                panic(msg);
            }

        private:
            VariantType _variant;
    };

}; // namespace lx::core
