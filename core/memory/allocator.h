#pragma once

#include <stddef.h>
#include <optional>
#include <memory>

#include "core/logger.h"

class Allocator {
public:
    static void* alloc(size_t bytes);
    static void* realloc(void* ptr, size_t bytes);
    static void free(void* ptr);

    template<typename T>
    static T* alloc(size_t count = 1) {
        return reinterpret_cast<T*>(alloc(sizeof(T) * count));
    }

    template<typename T>
    static T* realloc(T* ptr, size_t new_count = 1) {
        return reinterpret_cast<T*>(realloc(reinterpret_cast<void*>(ptr), sizeof(T) * new_count));
    }

    template<typename T>
    static void free(T* ptr) {
        free(reinterpret_cast<void*>(ptr));
    }

    template<typename T>
    struct Deleter {
        void operator()(T* ptr) {
            Allocator::free<T>(ptr);
        }
    };

    template<typename T, typename... Args>
    static std::shared_ptr<T> make_shared(Args&&... args) {
        T* ptr = alloc<T>();
        *ptr = T(args...);

        return std::shared_ptr<T>(ptr, Deleter<T>());
    }

private:
    struct Chunk {
        Chunk* p_next = nullptr;
    };

    struct Page {
        Chunk* p_begin = nullptr;
        size_t size = 0;

        Chunk* p_free = nullptr;
        Page* p_next = nullptr;
        size_t reservations = 0;

        enum : size_t {
            DefaultSize = 8000 // 64 kB
        };

        Page(size_t chunk_count = DefaultSize);
        ~Page();

        bool contains(const Chunk* p_chunk) const;
        std::optional<size_t> index_of(const Chunk* p_chunk) const;
        std::optional<size_t> reservation_size(Chunk* p_chunk) const;

        std::optional<Chunk*> reserve(size_t count);
        bool try_extend(Chunk* p_given_chunk, size_t count);
        void release(Chunk* p_given_chunk);
    };

    Page* p_page_begin = nullptr;
    Page* p_page_end = nullptr;

    static Allocator self;
};