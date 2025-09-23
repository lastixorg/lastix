#pragma once

#include <concepts>

namespace lx::trait {

    namespace impl {

        template <class...> inline constexpr bool always_false = false;
    };

    template <class FromType, class ToType> struct FromImpl;

    template <class FromType, class ToType>
    concept From = requires(FromType t) {
        { FromImpl<FromType, ToType>::from(t) } -> std::convertible_to<ToType>;
    };

}; // namespace lx::trait
