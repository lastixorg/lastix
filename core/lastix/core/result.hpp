#pragma once

#include "lastix/core/diagnostics.hpp"
#include "lastix/core/option.hpp"
#include "lastix/trait/from.hpp"

#include <variant>
#include <utility>

namespace lx::core {

    namespace impl {

        /**
         * @brief Tagged wrapper for Result values (Ok or Err).
         * @tparam T   Stored value type.
         * @tparam Tag Tag identifying whether this is OkTag or ErrTag.
         */
        template <class T, class Tag> class ResultTaggedValue {
            public:
                ResultTaggedValue() noexcept = default;

                /**
                 * @brief Constructs the tagged value.
                 * @param value Value to store (moved in).
                 */
                explicit ResultTaggedValue(T value) noexcept
                    : _value(std::move(value)) {
                }

                /**
                 * @brief Dereference operator returning the stored value.
                 * Preserves value category (lvalue/rvalue).
                 */
                template <class Self>
                auto operator*(this Self&& self) noexcept -> decltype(auto) {
                    return std::forward_like<Self>(self._value);
                }

                /**
                 * @brief Equality comparison.
                 */
                auto operator==(const ResultTaggedValue& other) const noexcept
                    -> bool {
                    return _value == other._value;
                }

            protected:
                T _value;
        };

        /**
         * @brief Specialization for ResultTaggedValue<void, Tag>.
         * Used for Ok<void> or Err<void> cases.
         */
        template <class Tag> class ResultTaggedValue<void, Tag> {

            public:
                ResultTaggedValue() = default;
        };

        /// Tag type representing successful result.
        struct OkTag {};

        /// Tag type representing failed result.
        struct ErrTag {};

        /// Deduction guide to handle nested Ok(Ok(...)).
        template <class U>
        ResultTaggedValue(ResultTaggedValue<U, OkTag>)
            -> ResultTaggedValue<ResultTaggedValue<U, OkTag>, OkTag>;

        /// Deduction guide to handle nested Err(Err(...)).
        template <class U>
        ResultTaggedValue(ResultTaggedValue<U, ErrTag>)
            -> ResultTaggedValue<ResultTaggedValue<U, ErrTag>, ErrTag>;

        /// Empty placeholder for void-type variants.
        struct Empty {};

        /// Helper to convert void → Empty for storage.
        template <class T> struct VoidOrTypeHelper {
                using type = T;
        };

        template <> struct VoidOrTypeHelper<void> {
                using type = Empty;
        };

        /// Shorthand alias for VoidOrTypeHelper.
        template <class T> using VoidOrType = VoidOrTypeHelper<T>::type;

        /// Concept requiring that a type has a context() method.
        template <class T>
        concept WithContext = requires(T t) { t.context(""); };

    }; // namespace impl

    using Empty = impl::Empty;

    /// Represents an Ok(value) success result.
    template <class T = void>
    using Ok = impl::ResultTaggedValue<T, impl::OkTag>;

    /// Represents an Err(error) failure result.
    template <class E = void>
    using Err = impl::ResultTaggedValue<E, impl::ErrTag>;

    /**
     * @brief Generic Result type representing either success (Ok) or failure
     * (Err).
     *
     * @tparam T Type of successful value.
     * @tparam E Type of error value.
     *
     * A Result is either Ok(T) or Err(E), and provides helpers for safe
     * unwrapping, context propagation, and conditional conversions.
     */
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

            /**
             * @brief Construct from convertible Ok<U>.
             * Accepts Ok<U> if U is convertible to Value and not From<U, T>.
             */
            template <class U>
            requires(!lx::trait::From<U, T> && std::convertible_to<U, Value>)
            Result(Ok<U> value) noexcept : _variant(*std::move(value)) {
            }

            /**
             * @brief Construct from convertible Err<F>.
             * Accepts Err<F> if F is convertible to Error and not From<F, E>.
             */
            template <class F>
            requires(!lx::trait::From<F, E> && std::convertible_to<F, Error>)
            Result(Err<F> error) noexcept : _variant(*std::move(error)) {
            }

            /// Construct from Ok<U> using trait-based conversion.
            template <class U>
            requires lx::trait::From<U, T>
            Result(Ok<U> value) noexcept
                : _variant(lx::trait::FromImpl<U, T>::from(*std::move(value))) {
            }

            /// Construct from Err<F> using trait-based conversion.
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

            /// @brief Returns true if the Result contains Ok(value).
            [[nodiscard]] auto is_ok() const noexcept -> bool {
                return std::holds_alternative<Value>(_variant);
            }

            /// @brief Returns true if the Result contains Err(error).
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

            /**
             * @brief Unwraps the Ok value or panics if Err.
             * @param loc Source location for diagnostics.
             */
            template <class Self>
            auto unwrap(this Self&& self,
                        std::source_location loc =
                            std::source_location::current()) noexcept
                -> decltype(auto) {
                return std::forward<Self>(self).expect("Called unwrap() on Err",
                                                       loc);
            }

            /**
             * @brief Unwraps the Ok value with custom panic message.
             */
            template <class Self>
            auto expect(this Self&& self, std::string_view msg,
                        std::source_location loc =
                            std::source_location::current()) noexcept
                -> decltype(auto) {
                if (auto* p = std::get_if<Value>(&self._variant))
                    return std::forward_like<Self>(*p);

                panic(msg, loc);
            }

            /**
             * @brief Unwraps the Err value or panics if Ok.
             */
            template <class Self>
            auto unwrap_err(this Self&& self,
                            std::source_location loc =
                                std::source_location::current()) noexcept
                -> decltype(auto) {
                return std::forward<Self>(self).expect_err(
                    "Called unwrap_err() on Ok", loc);
            }

            /**
             * @brief Unwraps the Err value with custom panic message.
             */
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

            /**
             * @brief Returns reference to the underlying std::variant.
             */
            template <class Self>
            auto get_variant(this Self&& self) noexcept -> decltype(auto) {
                return std::forward_like<Self>(self._variant);
            }

        private:
            VariantType _variant;
    };

}; // namespace lx::core
