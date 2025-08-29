#pragma once

#include "lastix/core/diagnostics.hpp"
#include "lastix/core/option.hpp"
#include "lastix/core/number.hpp"
#include "lastix/core/memory.hpp"
#include "lastix/trait/sync.hpp"

#include <atomic>
#include <utility>

namespace lx::core {

    namespace impl {

        struct ArcControlBlock {

                std::atomic<usize> strong_count = 0;
                std::atomic<usize> weak_count = 0;
        };

    }; // namespace impl

    template <class T, class Deleter = DefaultDeleter<T>> class Arc {

        public:
            template <class... Args>
            requires std::constructible_from<T, Args...>
            explicit Arc(Args&&... args) noexcept
                : _data(new T(std::forward<Args>(args)...)),
                  _cb(new impl::ArcControlBlock(1, 0)) {
            }

            Arc(Arc&& other) noexcept
                : _data(std::exchange(other._data, nullptr)),
                  _cb(std::exchange(other._cb, nullptr)) {
            }

            Arc(const Arc& other) noexcept
                : _data(other._data), _cb(other._cb) {

                if (_cb == nullptr) [[unlikely]]
                    return;

                _cb->strong_count.fetch_add(1, std::memory_order_acq_rel);
            }

            auto operator=(Arc&& other) noexcept -> Arc& {
                if (this != &other) {
                    this->reset();
                    _data = std::exchange(other._data, nullptr);
                    _cb = std::exchange(other._cb, nullptr);
                }

                return *this;
            }

            auto operator=(const Arc& other) noexcept -> Arc& {
                if (this != &other) {
                    this->reset();

                    if (other._cb != nullptr) {
                        other._cb->strong_count.fetch_add(
                            1, std::memory_order_acq_rel);
                        _data = other._data;
                        _cb = other._cb;
                    }
                }

                return *this;
            }

            template <class U>
            requires std::derived_from<U, T>
            explicit Arc(Arc<U>&& other) noexcept
                : _data(static_cast<T*>(std::exchange(other._data, nullptr))),
                  _cb(std::exchange(other._cb, nullptr)) {
            }

            template <class U>
            requires std::derived_from<U, T>
            explicit Arc(const Arc<U>& other) noexcept
                : _data(static_cast<T*>(other._data)), _cb(other._cb) {
                if (_cb == nullptr) [[unlikely]]
                    return;

                _cb->strong_count.fetch_add(1, std::memory_order_acq_rel);
            }

            template <class U>
            requires std::derived_from<U, T>
            auto operator=(Arc<U>&& other) -> Arc& {
                this->reset();
                _data = static_cast<T*>(std::exchange(other._data, nullptr));
                _cb = std::exchange(other._cb, nullptr);
                return *this;
            }

            template <class U>
            requires std::derived_from<U, T>
            auto operator=(const Arc<U>& other) -> Arc& {
                this->reset();
                if (other._cb) [[likely]] {

                    other._cb->strong_count.fetch_add(
                        1, std::memory_order_acq_rel);

                    _data = static_cast<T*>(other._data);
                    _cb = other._cb;
                }
                return *this;
            }

            ~Arc() noexcept {
                this->reset();
            }

            [[nodiscard]] static auto unsafe_from_raw(T* data) noexcept
                -> Arc<T, Deleter> {

                return Arc(data, new impl::ArcControlBlock(1, 0));
            }

            auto reset() noexcept -> void {

                if (_cb == nullptr) [[unlikely]]
                    return;

                if (_cb->strong_count.fetch_sub(1, std::memory_order_acq_rel) ==
                    1) {
                    Deleter{}(std::exchange(_data, nullptr));
                    if (_cb->weak_count.load(std::memory_order_acquire) == 0) {
                        delete std::exchange(_cb, nullptr);
                    }
                }
            }

            [[nodiscard]] auto operator->() const noexcept -> const T* {

                if (_data == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return _data;
            }

            [[nodiscard]] auto operator*() const noexcept -> const T& {

                if (_data == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return *_data;
            }

            template <class V = void>
            requires(lx::trait::Sync<T>)
            [[nodiscard]] auto operator->() noexcept -> T* {

                if (_data == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return _data;
            }

            template <class V = void>
            requires(lx::trait::Sync<T>)
            [[nodiscard]] auto operator*() noexcept -> T& {

                if (_data == nullptr) [[unlikely]]
                    panic("Dereferencing nullptr");

                return *_data;
            }

            [[nodiscard]] explicit operator bool() const noexcept {
                return _data != nullptr;
            }

            auto swap(Arc& other) noexcept -> void {
                std::swap(_data, other._data);
                std::swap(_cb, other._cb);
            }

            auto unsafe_get() const& noexcept -> const T* {
                return _data;
            }

            [[nodiscard]] auto strong_count() const noexcept -> Option<usize> {
                if (_cb == nullptr) [[unlikely]]
                    return None;

                return Some(_cb->strong_count.load(std::memory_order_acquire));
            }

            [[nodiscard]] auto weak_count() const noexcept -> Option<usize> {
                if (_cb == nullptr) [[unlikely]]
                    return None;

                return Some(_cb->weak_count.load(std::memory_order_acquire));
            }

        private:
            Arc(T* data, impl::ArcControlBlock* cb) noexcept
                : _data(data), _cb(cb) {
            }

            template <class U, class Del> friend class Arc;

        private:
            T* _data = nullptr;
            impl::ArcControlBlock* _cb = nullptr;
    };

}; // namespace lx::core
