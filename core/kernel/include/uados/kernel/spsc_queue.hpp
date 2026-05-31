#pragma once

/// @file spsc_queue.hpp
/// @brief Lock-free single-producer single-consumer bounded queue.
///
/// Used by the event bus for per-subscription message delivery.
/// Provides wait-free push/pop for one producer and one consumer thread.

#include <atomic>
#include <array>
#include <cstddef>
#include <optional>
#include <type_traits>

namespace uados::core {

/// @brief Lock-free SPSC (Single-Producer Single-Consumer) bounded queue.
///
/// Implementation uses a power-of-two sized ring buffer with separate
/// cache-line-aligned head and tail indices to avoid false sharing.
///
/// @tparam T Element type (must be movable)
/// @tparam Capacity Queue capacity (must be power of two)
template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

public:
    SPSCQueue() = default;

    /// Try to push an element (producer side)
    /// @param item Item to push
    /// @return true if successful, false if queue is full
    bool try_push(T item) noexcept(std::is_nothrow_move_constructible_v<T>) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (tail + 1) & kMask;

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // Queue full
        }

        buffer_[tail] = std::move(item);
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    /// Try to pop an element (consumer side)
    /// @return The element if available, std::nullopt if queue empty
    std::optional<T> try_pop() noexcept(std::is_nothrow_move_constructible_v<T>) {
        const size_t head = head_.load(std::memory_order_relaxed);

        if (head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt; // Queue empty
        }

        T item = std::move(buffer_[head]);
        head_.store((head + 1) & kMask, std::memory_order_release);
        return item;
    }

    /// Check if the queue is empty (approximate)
    [[nodiscard]] bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

    /// Check if the queue is full (approximate)
    [[nodiscard]] bool full() const noexcept {
        const size_t tail = tail_.load(std::memory_order_acquire);
        const size_t next_tail = (tail + 1) & kMask;
        return next_tail == head_.load(std::memory_order_acquire);
    }

    /// Approximate number of elements in the queue
    [[nodiscard]] size_t size_approx() const noexcept {
        const size_t head = head_.load(std::memory_order_acquire);
        const size_t tail = tail_.load(std::memory_order_acquire);
        return (tail - head) & kMask;
    }

    /// Maximum capacity
    [[nodiscard]] constexpr size_t capacity() const noexcept {
        return Capacity - 1; // One slot reserved for full detection
    }

private:
    static constexpr size_t kMask = Capacity - 1;

    // Cache line padding to prevent false sharing
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    std::array<T, Capacity> buffer_{};
};

} // namespace uados::core
