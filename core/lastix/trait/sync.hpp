#pragma once

#include <atomic>

namespace lx::trait {

    template <class T> struct UnsafeSyncMarker {
            static constexpr auto value = false;
    };

    template <class T> struct UnsafeSyncMarker<std::atomic<T>> {
            static constexpr auto value = true;
    };

    template <class T>
    concept Sync = UnsafeSyncMarker<T>::value;

}; // namespace lx::trait
