#pragma once

#include <stddef.h>
#include <optional>
#include <memory>

#include <source_location>
#include <set>
#include <map>
#include <thread>

// Motivation:  Displayable linked list nodes are allocated from the heap one at a time,
//              and that sucks. This allocator will completely prevent small allocations
//              by assigning memory from large pages, reducing the quantity of allocations.


class Allocator {
public:
    static void* alloc(size_t size_bytes);
    static void* realloc(void* p_addr, size_t new_size_bytes);
    static void free(void* p_addr);

    static size_t get_allocation_size(void* p_addr);

    static void setup();
    static void set_noop_free(bool enabled) { shared.noop_free_enabled = enabled; } // Allow for "instant" exit by not doing any bookkeeping
    static void cleanup();

    template<typename T, typename... Args>
    static std::shared_ptr<T> make_shared(Args... args) {
        struct Deleter {
            void operator()(T* p_addr) {
                p_addr->~T();
                Allocator::free(reinterpret_cast<void*>(p_addr));
            }
        };

        T* p_addr = new(Allocator::alloc(sizeof(T))) T(args...);
        return std::shared_ptr<T>(p_addr, Deleter());
    }

#ifndef TESTS_ENABLED
private:
#endif
    static constexpr size_t DEFAULT_PAGE_SIZE = 65536; // 64 KiB

    class Page {
    public:
        Page(size_t size_bytes);
        ~Page();

        std::optional<Page*> try_append_new_page(size_t size_bytes);

        Page* next() const { return p_next; }
        bool contains(void* p_addr) const;
        size_t reservation_size_bytes(void* p_addr) const;

        std::optional<void*> reserve(size_t size_bytes);
        bool resize(void* p_addr, size_t new_size_bytes);
        void release(void* p_addr);

        #ifdef DEBUG_ALLOC
        void ENSURE_CORRECTNESS(std::source_location src_loc = std::source_location::current());
        #else
        void ENSURE_CORRECTNESS() {}
        #endif

        Page(const Page&) = delete;
        Page(Page&&) = delete;

        Page& operator=(const Page&) = delete;
        Page& operator=(Page&&) = delete;
    
    #ifndef TESTS_ENABLED
    private:
    #endif
        struct Chunk {
            size_t contiguous_size = 0;
            Chunk* p_next = nullptr;
            
            Chunk() = default;
            ~Chunk() = default;

            Chunk(const Chunk&) = delete;
            Chunk(Chunk&&) = delete;

            Chunk& operator=(const Chunk&) = delete;
            Chunk& operator=(Chunk&&) = delete;
        };

        // Handle alignment
        void* chunk_to_void_addr(Chunk* p_chunk_addr) const;
        Chunk* void_to_chunk_addr(void* p_void_addr) const;
        Chunk* void_to_header_addr(void* p_void_addr) const;
        size_t bytes_to_chunk_count(size_t bytes) const;

        std::optional<Chunk*> get_first_contiguous(size_t contiguous_chunk_count);
        bool extend_contiguous_from(Chunk* p_from, size_t new_chunk_count);
        void reincorporate_contiguous(Chunk* p_chunk);

        void prepare_reservation(Chunk* p_chunk);
        void merge_reservations(Chunk* p_first, Chunk* p_second);
        void shrink_reservation(Chunk* p_chunk, size_t new_chunk_count);
        void close_reservation(Chunk* p_chunk);

        size_t chunk_count;
        Chunk* p_data = nullptr;
        Chunk* p_free = nullptr;
        Page* p_next = nullptr;

        #ifdef DEBUG_ALLOC
        std::set<Chunk*> DBG_chunks_reserved;
        #endif
        #ifndef NDEBUG
        std::thread::id _thread_id;
        #endif
    };

    bool noop_free_enabled = false;

    Page* p_page_begin = nullptr;
    Page* p_page_end = nullptr;

    Page* add_new_page(size_t size_bytes = DEFAULT_PAGE_SIZE);

    bool not_ready() const { return p_page_begin == nullptr || p_page_end == nullptr; }

    #ifdef DEBUG_ALLOC
    std::set<void*> DBG_addresses_allocated;
    void ENSURE_CORRECTNESS(std::source_location src_loc = std::source_location::current());
    #else
    void ENSURE_CORRECTNESS() {}
    #endif

    static Allocator shared;
};
