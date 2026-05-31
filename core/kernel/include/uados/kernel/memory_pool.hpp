#pragma once

/// @file memory_pool.hpp
/// @brief Lock-free memory pool allocator for real-time paths.
///
/// Provides pre-allocated, fixed-size memory blocks for zero-allocation
/// message passing on the event bus hot path. All memory is allocated
/// at startup; no malloc/free occurs during runtime operation.

#include "uados/types.hpp"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <vector>

namespace uados::core {

/// @brief Lock-free fixed-size block memory pool.
///
/// Uses a lock-free freelist (Treiber stack) for O(1) allocation
/// and deallocation without locks or system calls.
///
/// Thread safety: All operations are thread-safe and lock-free.
class MemoryPool {
public:
    /// Construct a pool with the given block size and count
    /// @param block_size Size of each block in bytes (will be aligned)
    /// @param block_count Total number of blocks to pre-allocate
    explicit MemoryPool(size_t block_size, size_t block_count)
        : block_size_(align_up(block_size, kAlignment)),
          block_count_(block_count),
          allocated_count_(0) {

        // Allocate the entire pool as one contiguous region
        const size_t node_size = sizeof(FreeNode) + block_size_;
        storage_.resize(node_size * block_count);

        // Initialize the free list
        for (size_t i = 0; i < block_count; ++i) {
            auto* node = node_at(i);
            new (node) FreeNode{}; // placement new
            node->next.store(
                (i + 1 < block_count) ? node_at(i + 1) : nullptr,
                std::memory_order_relaxed);
        }

        free_head_.store(node_at(0), std::memory_order_release);
    }

    ~MemoryPool() = default;

    // Non-copyable, non-movable
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    /// Allocate a block from the pool (lock-free)
    /// @return Pointer to allocated block, or nullptr if pool exhausted
    [[nodiscard]] void* allocate() noexcept {
        FreeNode* old_head = free_head_.load(std::memory_order_acquire);

        while (old_head != nullptr) {
            FreeNode* next = old_head->next.load(std::memory_order_relaxed);
            if (free_head_.compare_exchange_weak(
                    old_head, next,
                    std::memory_order_release,
                    std::memory_order_acquire)) {
                allocated_count_.fetch_add(1, std::memory_order_relaxed);
                return block_from_node(old_head);
            }
            // CAS failed, old_head updated, retry
        }

        return nullptr; // Pool exhausted
    }

    /// Deallocate a block back to the pool (lock-free)
    /// @param ptr Pointer previously returned by allocate()
    void deallocate(void* ptr) noexcept {
        if (ptr == nullptr) return;

        auto* node = node_from_block(ptr);

        FreeNode* old_head = free_head_.load(std::memory_order_acquire);
        do {
            node->next.store(old_head, std::memory_order_relaxed);
        } while (!free_head_.compare_exchange_weak(
            old_head, node,
            std::memory_order_release,
            std::memory_order_acquire));

        allocated_count_.fetch_sub(1, std::memory_order_relaxed);
    }

    /// Allocate and construct an object
    template <typename T, typename... Args>
    [[nodiscard]] T* construct(Args&&... args) {
        static_assert(sizeof(T) <= 65536, "Object too large for pool");
        void* mem = allocate();
        if (mem == nullptr) return nullptr;
        return new (mem) T(std::forward<Args>(args)...);
    }

    /// Destroy and deallocate an object
    template <typename T>
    void destroy(T* obj) {
        if (obj == nullptr) return;
        obj->~T();
        deallocate(obj);
    }

    // -- Statistics --

    /// Block size (aligned)
    [[nodiscard]] size_t block_size() const noexcept { return block_size_; }

    /// Total number of blocks
    [[nodiscard]] size_t block_count() const noexcept { return block_count_; }

    /// Currently allocated blocks
    [[nodiscard]] size_t allocated() const noexcept {
        return allocated_count_.load(std::memory_order_relaxed);
    }

    /// Available (free) blocks
    [[nodiscard]] size_t available() const noexcept {
        return block_count_ - allocated();
    }

    /// Pool utilization [0.0, 1.0]
    [[nodiscard]] double utilization() const noexcept {
        if (block_count_ == 0) return 0.0;
        return static_cast<double>(allocated()) / static_cast<double>(block_count_);
    }

    /// Total memory footprint in bytes
    [[nodiscard]] size_t total_bytes() const noexcept {
        return storage_.size();
    }

    /// Check if a pointer belongs to this pool
    [[nodiscard]] bool owns(const void* ptr) const noexcept {
        const auto* p = static_cast<const uint8_t*>(ptr);
        const auto* begin = storage_.data();
        const auto* end = begin + storage_.size();
        return p >= begin && p < end;
    }

private:
    static constexpr size_t kAlignment = 64; // Cache line alignment

    struct FreeNode {
        std::atomic<FreeNode*> next{nullptr};
    };

    static constexpr size_t align_up(size_t n, size_t alignment) noexcept {
        return (n + alignment - 1) & ~(alignment - 1);
    }

    FreeNode* node_at(size_t index) noexcept {
        const size_t node_size = sizeof(FreeNode) + block_size_;
        return reinterpret_cast<FreeNode*>(storage_.data() + index * node_size);
    }

    void* block_from_node(FreeNode* node) noexcept {
        return reinterpret_cast<uint8_t*>(node) + sizeof(FreeNode);
    }

    FreeNode* node_from_block(void* block) noexcept {
        return reinterpret_cast<FreeNode*>(
            static_cast<uint8_t*>(block) - sizeof(FreeNode));
    }

    size_t block_size_;
    size_t block_count_;
    std::atomic<size_t> allocated_count_;
    std::atomic<FreeNode*> free_head_{nullptr};
    std::vector<uint8_t> storage_;
};

/// @brief Multi-size memory pool manager.
///
/// Manages multiple pools of different block sizes for efficient
/// allocation of varying-size messages on the event bus.
class PoolAllocator {
public:
    /// Configuration for a single pool tier
    struct PoolTier {
        size_t block_size;
        size_t block_count;
    };

    /// Construct with a set of pool tiers
    /// @param tiers Pool configurations, sorted by block_size ascending
    explicit PoolAllocator(std::vector<PoolTier> tiers) {
        pools_.reserve(tiers.size());
        for (auto& tier : tiers) {
            pools_.push_back(std::make_unique<MemoryPool>(
                tier.block_size, tier.block_count));
        }
    }

    /// Default pool configuration for UADOS event bus
    static PoolAllocator create_default() {
        return PoolAllocator({
            {64,     16384},   // 1MB total — small messages (commands, status)
            {256,    8192},    // 2MB total — medium messages (detections, poses)
            {1024,   4096},    // 4MB total — large messages (point subsets)
            {4096,   2048},    // 8MB total — very large (radar scans)
            {65536,  512},     // 32MB total — huge (small images)
            {262144, 128},     // 32MB total — extra large (medium images)
            {1048576, 32},     // 32MB total — maximum (full images)
        });
    }

    /// Allocate from the smallest pool that fits
    [[nodiscard]] void* allocate(size_t size) noexcept {
        for (auto& pool : pools_) {
            if (pool->block_size() >= size) {
                void* ptr = pool->allocate();
                if (ptr != nullptr) return ptr;
            }
        }
        return nullptr; // No pool has capacity
    }

    /// Deallocate back to the owning pool
    void deallocate(void* ptr) noexcept {
        for (auto& pool : pools_) {
            if (pool->owns(ptr)) {
                pool->deallocate(ptr);
                return;
            }
        }
        // ptr doesn't belong to any pool — this is a bug
        assert(false && "Deallocating pointer not owned by any pool");
    }

    /// Get statistics for all pools
    struct PoolStats {
        size_t block_size;
        size_t total;
        size_t allocated;
        size_t available;
        double utilization;
    };

    [[nodiscard]] std::vector<PoolStats> stats() const {
        std::vector<PoolStats> result;
        result.reserve(pools_.size());
        for (const auto& pool : pools_) {
            result.push_back({
                pool->block_size(),
                pool->block_count(),
                pool->allocated(),
                pool->available(),
                pool->utilization()
            });
        }
        return result;
    }

    /// Total memory footprint
    [[nodiscard]] size_t total_bytes() const noexcept {
        size_t total = 0;
        for (const auto& pool : pools_) {
            total += pool->total_bytes();
        }
        return total;
    }

private:
    std::vector<std::unique_ptr<MemoryPool>> pools_;
};

} // namespace uados::core
