#pragma once

#include "lastix/core/option.hpp"
#include "lastix/core/box.hpp"

#include <string_view>
#include <string>
#include <type_traits>

namespace lx::core {

    namespace impl {

        struct ErrorBase {
                virtual ~ErrorBase() noexcept = default;
                virtual auto what() const noexcept -> std::string_view = 0;
        };

        class StringError : public ErrorBase {

            public:
                StringError(std::string err);

                auto what() const noexcept -> std::string_view override;

            protected:
                std::string _err;
        };

    }; // namespace impl

    class Error {

        public:
            // template <class E>
            // Error(E e) noexcept : _inner(Box<E>(std::move(e))) {
            // }

            template <class T>
            requires std::convertible_to<T, std::string>
            Error(T e) noexcept : _inner(Box<impl::StringError>(std::move(e))) {
            }

            auto context(std::string_view msg) noexcept -> Error;

            auto what() const noexcept -> std::string_view;

            template <class F>
            requires std::is_invocable_v<F, std::string_view>
            auto write(F&& f) const
                noexcept(std::is_nothrow_invocable_v<F, std::string_view>)
                    -> void {

                f(_inner->what());
                const auto* next = &_next;

                while (next->is_some()) {
                    f(next->unwrap()->what());
                    next = (&next->unwrap()->_next);
                }
            }

        protected:
            Box<impl::ErrorBase> _inner;
            Option<Box<Error>> _next = None;
    };


}; // namespace lx::core
