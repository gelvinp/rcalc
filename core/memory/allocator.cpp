#include "allocator.h"

#include <algorithm>

#include "core/logger.h"


void* Allocator::alloc(size_t bytes) {
    static_assert(sizeof(size_t) == sizeof(Chunk));
    if (bytes == 0) { return nullptr; }

    size_t chunk_count = std::ceil(std::max(bytes, (size_t)8) / 8.0);

    // Allocate initial page
    if (self.p_page_begin == nullptr) {
        size_t page_chunks = std::max(chunk_count, (size_t)Page::DefaultSize);
        self.p_page_begin = new Page(page_chunks);
    }
    if (self.p_page_end == nullptr) {
        self.p_page_end = self.p_page_begin;
    }

    // Reserve chunks
    std::optional<Chunk*> reserved = self.p_page_begin->reserve(chunk_count);
    if (!reserved.has_value()) {
        RCalc::Logger::log_info("Could not reserve value, allocating new page...");
        
        // Allocate new page
        size_t page_chunks = std::max(chunk_count, (size_t)Page::DefaultSize);
        self.p_page_end->p_next = new Page(page_chunks);
        self.p_page_end = self.p_page_end->p_next;
        reserved = self.p_page_end->reserve(chunk_count);

        if (!reserved.has_value()) {
            RCalc::Logger::log_err("Unable to reserve chunks, even with new page!");
            return nullptr;
        }
    }
    
    return reinterpret_cast<void*>(reserved.value());
}


void* Allocator::realloc(void* ptr, size_t bytes) {
    static_assert(sizeof(size_t) == sizeof(Chunk));
    if (bytes == 0) { return nullptr; }
    if (ptr == nullptr) { return nullptr; }
    if (self.p_page_begin == nullptr) { return nullptr; }

    size_t chunk_count = std::ceil(std::max(bytes, (size_t)8) / 8.0);

    Chunk* p_chunk = reinterpret_cast<Chunk*>(ptr);

    // Make sure given pointer exists within a page
    for (Page* p_page = self.p_page_begin; p_page != nullptr; p_page = p_page->p_next) {
        if (p_page->contains(p_chunk)) {
            if (p_page->try_extend(p_chunk, chunk_count)) {
                return ptr;
            }
            else {
                // Cannot extend, allocate new and then move
                void* p_new = alloc(bytes);
                memcpy(p_new, ptr, bytes);
                return p_new;
            }
        }
    }

    // Pointer was not found in any given page, do nothing
    return nullptr;
}


void Allocator::free(void* ptr) {
    static_assert(sizeof(size_t) == sizeof(Chunk));
    if (ptr == nullptr) { return; }
    if (self.p_page_begin == nullptr) { return; }

    Chunk* p_chunk = reinterpret_cast<Chunk*>(ptr);

    // Make sure given pointer exists within a page
    for (Page* p_page = self.p_page_begin; p_page != nullptr; p_page = p_page->p_next) {
        if (p_page->contains(p_chunk)) {
            p_page->release(p_chunk);
            return;
        }
    }
}


Allocator::Page::Page(size_t chunk_count)
    : size(chunk_count + 1) // Allocate one extra so we can store length information in the > default case
{
    p_begin = new Chunk[size];
    p_free = p_begin;

    for (size_t index = 0; index < (size - 1); ++index) {
        p_begin[index].p_next = &p_begin[index + 1];
    }
}

Allocator::Page::~Page() {
    if (reservations > 0) {
        RCalc::Logger::log_err("Page freed while reservations still exist!");
    }
    delete[] p_begin;

    if (p_next) {
        delete p_next;
    }
}

bool Allocator::Page::contains(const Chunk* p_chunk) const {
    return (p_chunk >= p_begin) && (p_chunk < (p_begin + size));
}

std::optional<size_t> Allocator::Page::index_of(const Chunk* p_chunk) const {
    if (!contains(p_chunk)) { return std::nullopt; }
    return (size_t)(p_chunk - p_begin);
}

std::optional<size_t> Allocator::Page::reservation_size(Chunk* p_chunk) const {
    if (!contains(p_chunk)) { return std::nullopt; }
    
    Chunk* p_size_chunk = p_chunk - 1;
    if (!contains(p_size_chunk)) { return std::nullopt; }

    return *reinterpret_cast<size_t*>(p_size_chunk);
}

std::optional<Allocator::Chunk*> Allocator::Page::reserve(size_t count) {
    static_assert(sizeof(size_t) == sizeof(Chunk));
    if (p_free == nullptr) { return std::nullopt; }

    size_t chunks_needed = count + 1;

    // Find first contiguous segment of `count` chunks
    size_t contiguous_size = 0;
    Chunk* p_contiguous_before = nullptr;
    Chunk* p_contiguous_begin = nullptr;
    Chunk* p_contiguous_after = nullptr;

    for (Chunk* p_chunk = p_free; p_chunk != nullptr; p_chunk = p_chunk->p_next) {
        if ((p_chunk + 1) != p_chunk->p_next) {
            // Not contiguous anymore
            contiguous_size = 0;
            p_contiguous_before = p_contiguous_begin;
            p_contiguous_begin = nullptr;
            continue;
        }

        if (p_contiguous_begin == nullptr) { p_contiguous_begin = p_chunk; }
        
        if (++contiguous_size >= chunks_needed) {
            p_contiguous_after = p_chunk->p_next;
            break;
        }
    }

    if (p_contiguous_begin == nullptr) { return std::nullopt; }

    // Found contiguous region
    Chunk* p_ret = p_contiguous_begin->p_next;

    if (p_contiguous_before) {
        p_contiguous_before->p_next = p_contiguous_after;
    }
    else {
        p_free = p_contiguous_after;
    }
    
    memset(reinterpret_cast<void*>(p_contiguous_begin), 0, sizeof(Chunk) * contiguous_size);
    *reinterpret_cast<size_t*>(p_contiguous_begin) = count;
    reservations++;

    return p_ret;
}

bool Allocator::Page::try_extend(Chunk* p_given_chunk, size_t count) {
    static_assert(sizeof(size_t) == sizeof(Chunk));
    if (p_free == nullptr) { return true; }
    if (!contains(p_given_chunk)) { return true; }
    
    Chunk* p_size_chunk = p_given_chunk - 1;
    if (!contains(p_size_chunk)) { return true; }

    size_t contiguous_size = *reinterpret_cast<size_t*>(p_size_chunk) + 1;
    size_t chunks_needed = count + 1;

    if (chunks_needed == contiguous_size) {
        // No adjustment necessary
        return true;
    }
    else if (chunks_needed < contiguous_size) {
        // Todo: Shrink!
        size_t shrink_needed = contiguous_size - chunks_needed;

        Chunk* p_shrink_begin = p_size_chunk + chunks_needed;
        Chunk* p_shrink_end = p_shrink_begin + shrink_needed - 1;

        memset(reinterpret_cast<void*>(p_shrink_begin), 0, sizeof(Chunk) * shrink_needed);

        for (Chunk* p_chunk = p_shrink_begin; p_chunk < p_shrink_end; ++p_chunk) {
            p_chunk->p_next = p_chunk + 1;
        }

        Chunk* p_shrink_before = nullptr;
        Chunk* p_shrink_after = nullptr;
        
        for (Chunk* p_chunk = p_free; p_chunk != nullptr; p_chunk = p_chunk->p_next) {
            if (p_chunk->p_next <= p_shrink_begin) {
                p_shrink_before = p_chunk;
            }
            else if (p_chunk >= p_shrink_end) {
                p_shrink_after = p_chunk;
                break;
            }
        }

        if (p_shrink_before) {
            p_shrink_before->p_next = p_shrink_begin;
        }
        else {
            p_free = p_shrink_begin;
        }

        p_shrink_end->p_next = p_shrink_after;

        return true;
    }

    // Check for grow
    size_t new_contiguous_needed = chunks_needed - contiguous_size;

    Chunk* p_contiguous_begin = p_size_chunk;
    Chunk* p_contiguous_end = p_contiguous_begin + contiguous_size - 1;
    Chunk* p_contiguous_after = p_contiguous_end + 1;

    Chunk* p_new_contiguous_from = nullptr;
    Chunk* p_new_contiguous_before = nullptr;
    Chunk* p_new_contiguous_after = nullptr;
    size_t p_new_contiguous_size = 0;

    for (Chunk* p_chunk = p_free; p_chunk != nullptr; p_chunk = p_chunk->p_next) {
        if (p_new_contiguous_from) {
            if ((p_chunk + 1) != p_chunk->p_next) {
                // Not contiguous anymore
                p_new_contiguous_size = 0;
                p_new_contiguous_from = nullptr;
                break;
            }
        }
        else {
            if (p_chunk == p_contiguous_after) {
                p_new_contiguous_from = p_chunk;
            }
        }

        if (++p_new_contiguous_size >= new_contiguous_needed) {
            p_new_contiguous_after = p_chunk->p_next;
            break;
        }
    }

    if (p_new_contiguous_from == nullptr) { return false; }

    if (p_new_contiguous_before) {
        p_new_contiguous_before->p_next = p_new_contiguous_after;
    }
    else {
        p_free = p_new_contiguous_after;
    }

    memset(reinterpret_cast<void*>(p_new_contiguous_from), 0, sizeof(Chunk) * new_contiguous_needed);
    *reinterpret_cast<size_t*>(p_size_chunk) = count;

    return true;
}

void Allocator::Page::release(Chunk* p_given_chunk) {
    static_assert(sizeof(size_t) == sizeof(Chunk));
    if (p_free == nullptr) { return; }
    if (!contains(p_given_chunk)) { return; }
    
    Chunk* p_size_chunk = p_given_chunk - 1;
    if (!contains(p_size_chunk)) { return; }

    size_t contiguous_size = *reinterpret_cast<size_t*>(p_size_chunk) + 1;

    Chunk* p_contiguous_begin = p_size_chunk;
    Chunk* p_contiguous_end = p_contiguous_begin + contiguous_size - 1;

    memset(reinterpret_cast<void*>(p_contiguous_begin), 0, sizeof(Chunk) * contiguous_size);

    for (Chunk* p_chunk = p_contiguous_begin; p_chunk < p_contiguous_end; ++p_chunk) {
        p_chunk->p_next = p_chunk + 1;
    }

    Chunk* p_contiguous_before = nullptr;
    Chunk* p_contiguous_after = nullptr;
    
    for (Chunk* p_chunk = p_free; p_chunk != nullptr; p_chunk = p_chunk->p_next) {
        if (p_chunk->p_next <= p_contiguous_begin) {
            p_contiguous_before = p_chunk;
        }
        else if (p_chunk >= p_contiguous_end) {
            p_contiguous_after = p_chunk;
            break;
        }
    }

    if (p_contiguous_before) {
        p_contiguous_before->p_next = p_contiguous_begin;
    }
    else {
        p_free = p_contiguous_begin;
    }

    p_contiguous_end->p_next = p_contiguous_after;

    reservations--;
}

Allocator Allocator::self;