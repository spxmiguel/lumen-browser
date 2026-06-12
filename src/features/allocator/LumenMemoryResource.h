#pragma once
#include <memory_resource>
#include <atomic>
#include <new>
#include <cstddef>

// C++20 polymorphic allocator resource with per-instance byte budget.
// Usage:
//   LumenMemoryResource pool{64 * 1024 * 1024};  // 64 MB budget
//   std::pmr::vector<int> v{&pool};
class LumenMemoryResource final : public std::pmr::memory_resource {
public:
    explicit LumenMemoryResource(
        size_t budget_bytes,
        std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : upstream_(upstream), budget_(budget_bytes) {}

    // Bytes currently live (allocated but not yet freed).
    size_t used() const noexcept { return used_.load(std::memory_order_relaxed); }
    size_t budget() const noexcept { return budget_; }

    // Remaining headroom — can be negative if budget changed at runtime.
    ptrdiff_t headroom() const noexcept {
        return static_cast<ptrdiff_t>(budget_) -
               static_cast<ptrdiff_t>(used_.load(std::memory_order_relaxed));
    }

    // Dynamically update the budget (e.g. when user switches modes at runtime).
    void set_budget(size_t bytes) noexcept { budget_ = bytes; }

private:
    void* do_allocate(size_t bytes, size_t align) override {
        size_t prev = used_.fetch_add(bytes, std::memory_order_relaxed);
        if (budget_ > 0 && prev + bytes > budget_) {
            used_.fetch_sub(bytes, std::memory_order_relaxed);
            throw std::bad_alloc{};
        }
        return upstream_->allocate(bytes, align);
    }

    void do_deallocate(void* ptr, size_t bytes, size_t align) override {
        upstream_->deallocate(ptr, bytes, align);
        used_.fetch_sub(bytes, std::memory_order_relaxed);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    std::pmr::memory_resource* upstream_;
    size_t                     budget_;
    std::atomic<size_t>        used_{0};
};
