#pragma once

#include "lastix/core/diagnostics.cpp"
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

    }; // namespace impl

    template <class T> using Ok = impl::ResultEnumeration<T, impl::OkTag>;
    template <class E> using Err = impl::ResultEnumeration<E, impl::ErrTag>;

    template <class T, class E> class Result {
        public:
            using Value = T;
            using Error = E;
            using VariantType = std::variant<Ok<T>, Err<E>>;

            Result(Ok<T> value) noexcept : _variant(std::move(value)) {
            }
            Result(Err<E> error) noexcept : _variant(std::move(error)) {
            }

            /// Accept convertible Ok<U> if U -> T
            template <class U>
            requires std::convertible_to<U, T>
            Result(Ok<U> value) noexcept : _variant(Ok<T>{*std::move(value)}) {
            }

            /// Accept convertible Err<F> if F -> E
            template <class F>
            requires std::convertible_to<F, E>
            Result(Err<F> error) noexcept
                : _variant(Err<E>{*std::move(error)}) {
            }

            [[nodiscard]] auto is_ok() const noexcept -> bool {
                return std::holds_alternative<Ok<T>>(_variant);
            }

            [[nodiscard]] auto is_err() const noexcept -> bool {
                return std::holds_alternative<Err<E>>(_variant);
            }

            [[nodiscard]] auto ok() const& noexcept -> Option<T> {
                if (auto* p = std::get_if<Ok<T>>(&_variant)) return Some(**p);

                return None;
            }

            [[nodiscard]] auto ok() && noexcept -> Option<T> {
                if (auto* p = std::get_if<Ok<T>>(&_variant))
                    return Some(std::move(**p));

                return None;
            }

            [[nodiscard]] auto err() const& noexcept -> Option<E> {
                if (auto* p = std::get_if<Err<E>>(&_variant)) return Some(**p);

                return None;
            }

            [[nodiscard]] auto err() && noexcept -> Option<E> {
                if (auto* p = std::get_if<Err<E>>(&_variant))
                    return Some(std::move(**p));

                return None;
            }

            auto unwrap() & noexcept -> T& {
                if (auto* p = std::get_if<Ok<T>>(&_variant)) return **p;

                panic("Called unwrap() on Err");
            }

            auto unwrap() const& noexcept -> const T& {
                if (auto* p = std::get_if<Ok<T>>(&_variant)) return **p;

                panic("Called unwrap() on Err");
            }

            auto unwrap() && noexcept -> T&& {
                if (auto* p = std::get_if<Ok<T>>(&_variant))
                    return std::move(**p);

                panic("Called unwrap() on Err");
            }

            auto expect(std::string_view msg) & noexcept -> T& {
                if (auto* p = std::get_if<Ok<T>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect(std::string_view msg) const& noexcept -> const T& {
                if (auto* p = std::get_if<Ok<T>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect(std::string_view msg) && noexcept -> T&& {
                if (auto* p = std::get_if<Ok<T>>(&_variant))
                    return std::move(**p);

                panic(msg);
            }

            auto unwrap_err() & noexcept -> E& {
                if (auto* p = std::get_if<Err<E>>(&_variant)) return **p;

                panic("Called unwrap_err() on Ok");
            }

            auto unwrap_err() const& noexcept -> const E& {
                if (auto* p = std::get_if<Err<E>>(&_variant)) return **p;

                panic("Called unwrap_err() on Ok");
            }

            auto unwrap_err() && noexcept -> E&& {
                if (auto* p = std::get_if<Err<E>>(&_variant))
                    return std::move(**p);

                panic("Called unwrap_err() on Ok");
            }

            auto expect_err(std::string_view msg) & noexcept -> E& {
                if (auto* p = std::get_if<Err<E>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect_err(std::string_view msg) const& noexcept -> const E& {
                if (auto* p = std::get_if<Err<E>>(&_variant)) return **p;

                panic(msg);
            }

            auto expect_err(std::string_view msg) && noexcept -> E&& {
                if (auto* p = std::get_if<Err<E>>(&_variant))
                    return std::move(**p);

                panic(msg);
            }

        private:
            VariantType _variant;
    };

}; // namespace lx::core
