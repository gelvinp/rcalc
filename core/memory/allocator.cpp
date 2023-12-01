#include "allocator.h"

#include "core/logger.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>


void* Allocator::alloc(size_t size_bytes) {
    if (shared.not_ready()) { shared.setup(); }
    shared.ENSURE_CORRECTNESS();

    for (Page* p_page = shared.p_page_begin; p_page != nullptr; p_page = p_page->next()) {
        std::optional<void*> op_addr = p_page->reserve(size_bytes);
        
        if (op_addr.has_value()) {
            #ifdef DEBUG_ALLOC
            shared.DBG_addresses_allocated.insert(op_addr.value());
            shared.ENSURE_CORRECTNESS();
            #endif
            return op_addr.value();
        }
    }

    void* p_addr = shared.add_new_page(size_bytes)->reserve(size_bytes).value();
    #ifdef DEBUG_ALLOC
    shared.DBG_addresses_allocated.insert(p_addr);
    shared.ENSURE_CORRECTNESS();
    #endif
    return p_addr;
}

void* Allocator::realloc(void* p_addr, size_t new_size_bytes) {
    if (shared.not_ready()) {
        throw std::logic_error("Cannot reallocate before allocator is setup!");
    }

    #ifdef DEBUG_ALLOC
    if (!shared.DBG_addresses_allocated.contains(p_addr)) {
        throw std::logic_error("Cannot reallocate an address never allocated!");
    }
    #endif

    shared.ENSURE_CORRECTNESS();

    for (Page* p_page = shared.p_page_begin; p_page != nullptr; p_page = p_page->next()) {
        if (p_page->contains(p_addr)) {
            // Try to resize in place
            if (p_page->resize(p_addr, new_size_bytes)) {
                shared.ENSURE_CORRECTNESS();
                return p_addr;
            }
            else {
                // Allocate new, copy, and release
                void* p_new_addr = alloc(new_size_bytes);
                size_t copy_amount = std::min(new_size_bytes, p_page->reservation_size_bytes(p_addr));
                memmove(p_new_addr, p_addr, copy_amount);
                free(p_addr);
                shared.ENSURE_CORRECTNESS();
                return p_new_addr;
            }
        }
    }

    throw std::logic_error("Cannot reallocate address not claimed by any page!");
}

void Allocator::free(void* p_addr) {
    if (shared.not_ready()) {
        throw std::logic_error("Cannot free before allocator is setup!");
    }

    if (shared.noop_free_enabled) { return; }

    #ifdef DEBUG_ALLOC
    if (!shared.DBG_addresses_allocated.contains(p_addr)) {
        throw std::logic_error("Cannot free an address never allocated!");
    }
    #endif

    shared.ENSURE_CORRECTNESS();

    for (Page* p_page = shared.p_page_begin; p_page != nullptr; p_page = p_page->next()) {
        if (p_page->contains(p_addr)) {
            p_page->release(p_addr);
            shared.ENSURE_CORRECTNESS();
            return;
        }
    }
}


size_t Allocator::get_allocation_size(void* p_addr) {
    if (shared.not_ready()) {
        throw std::logic_error("Cannot get allocation size before allocator is setup!");
    }

    #ifdef DEBUG_ALLOC
    if (!shared.DBG_addresses_allocated.contains(p_addr)) {
        throw std::logic_error("Cannot get allocation size for an address never allocated!");
    }
    #endif

    shared.ENSURE_CORRECTNESS();

    for (Page* p_page = shared.p_page_begin; p_page != nullptr; p_page = p_page->next()) {
        if (p_page->contains(p_addr)) {
            return p_page->reservation_size_bytes(p_addr);
        }
    }
}


void Allocator::setup() {
    if (!shared.not_ready()) { return; }

    shared.p_page_begin = shared.add_new_page();
}

void Allocator::cleanup() {
    delete shared.p_page_begin;
    shared.p_page_begin = nullptr;
    shared.p_page_end = nullptr;
}


std::optional<Allocator::Page*> Allocator::get_page_containing(void* p_addr) const {
    if (not_ready()) { return std::nullopt; }

    for (Page* p_page = p_page_begin; p_page != nullptr; p_page = p_page->next()) {
        if (p_page->contains(p_addr)) { return p_page; }
    }

    return std::nullopt;
}

Allocator::Page* Allocator::add_new_page(size_t size_bytes) {
    size_t page_size_bytes = std::max(size_bytes, DEFAULT_PAGE_SIZE);

    if (p_page_end != nullptr) {
        p_page_end = p_page_end->try_append_new_page(page_size_bytes).value();
    }
    else {
        p_page_end = new Page(page_size_bytes);
    }

    return p_page_end;
}


Allocator::Page::Page(size_t size_bytes) {
    chunk_count = bytes_to_chunk_count(size_bytes) + 1;
    if (chunk_count == 0) {
        throw std::logic_error("Cannot allocate a zero-chunk page!");
    }

    p_data = new Chunk[chunk_count];
    p_data[0].contiguous_size = chunk_count;
    p_free = p_data;
    #ifndef NDEBUG
    _thread_id = std::this_thread::get_id();
    #endif
}

Allocator::Page::~Page() {
    if (p_data != nullptr) {
        delete[] p_data;
    }
    if (p_next != nullptr) {
        delete p_next;
    }
}

std::optional<Allocator::Page*> Allocator::Page::try_append_new_page(size_t size_bytes) {
    if (p_next != nullptr) {
        return std::nullopt;
    }

    #ifndef NDEBUG
    if (size_bytes > DEFAULT_PAGE_SIZE) {
        RCalc::Logger::log_info("Requesting large page: %d bytes", size_bytes);
    }
    #endif

    p_next = new Page(size_bytes);
    return p_next;
}


bool Allocator::Page::contains(void* p_addr) const {
    Chunk* p_chunk = void_to_chunk_addr(p_addr);

    if (p_chunk < p_data) { return false; }
    if (p_chunk >= p_data + chunk_count) { return false; }

    return true;
}

size_t Allocator::Page::reservation_size_bytes(void* p_addr) const {
    return void_to_header_addr(p_addr)->contiguous_size * sizeof(Chunk);
}


std::optional<void*> Allocator::Page::reserve(size_t size_bytes) {
    #ifndef NDEBUG
    if (_thread_id != std::this_thread::get_id()) {
        throw std::logic_error("Allocator is not currently thread safe!");
    }
    #endif
    size_t chunk_count = bytes_to_chunk_count(size_bytes);
    std::optional<Chunk*> op_chunk = get_first_contiguous(chunk_count);
    
    if (!op_chunk.has_value()) {
        return std::nullopt;
    }

    if (void_to_chunk_addr(chunk_to_void_addr(op_chunk.value())) != op_chunk.value()) {
        throw std::logic_error("Cannot convert through void*!");
    }
    if (void_to_header_addr(chunk_to_void_addr(op_chunk.value()))->p_next != op_chunk.value()) {
        throw std::logic_error("Header is malformed!");
    }

    return chunk_to_void_addr(op_chunk.value());
}

bool Allocator::Page::resize(void* p_addr, size_t new_size_bytes) {
    #ifndef NDEBUG
    if (_thread_id != std::this_thread::get_id()) {
        throw std::logic_error("Allocator is not currently thread safe!");
    }
    #endif
    Chunk* p_chunk = void_to_header_addr(p_addr);
    size_t current_chunk_count = p_chunk->contiguous_size;
    size_t new_chunk_count = bytes_to_chunk_count(new_size_bytes);

    if (new_chunk_count == current_chunk_count) {
        // Nothing to do
        return true;
    }
    else if (new_chunk_count < current_chunk_count) {
        // Shrink
        shrink_reservation(p_chunk, new_chunk_count);
        return true;
    }
    else {
        // Attempt to grow reservation
        return extend_contiguous_from(p_chunk, new_chunk_count);
    }
}

void Allocator::Page::release(void* p_addr) {
    #ifndef NDEBUG
    if (_thread_id != std::this_thread::get_id()) {
        throw std::logic_error("Allocator is not currently thread safe!");
    }
    #endif
    Chunk* p_chunk = void_to_header_addr(p_addr);
    if (!contains(p_chunk)) {
        throw std::logic_error("Cannot release a chunk not contained by this page!");
    }
    reincorporate_contiguous(p_chunk);
}


void* Allocator::Page::chunk_to_void_addr(Chunk* p_chunk_addr) const {
    return reinterpret_cast<void*>(p_chunk_addr);
}

Allocator::Page::Chunk* Allocator::Page::void_to_chunk_addr(void* p_void_addr) const {
    uintptr_t unaligned = reinterpret_cast<uintptr_t>(p_void_addr);
    uintptr_t misalignment = unaligned % sizeof(Chunk);
    return reinterpret_cast<Chunk*>(unaligned - misalignment);
}

Allocator::Page::Chunk* Allocator::Page::void_to_header_addr(void* p_void_addr) const {
    uintptr_t unaligned = reinterpret_cast<uintptr_t>(p_void_addr);
    uintptr_t misalignment = unaligned % sizeof(Chunk);
    return reinterpret_cast<Chunk*>(unaligned - misalignment) - 1;
}

size_t Allocator::Page::bytes_to_chunk_count(size_t bytes) const {
    return (size_t)(std::ceil(double(bytes) / double(sizeof(Chunk))));
}


std::optional<Allocator::Page::Chunk*> Allocator::Page::get_first_contiguous(size_t chunk_count) {
    ENSURE_CORRECTNESS();

    size_t chunk_count_with_header = chunk_count + 1;

    Chunk* p_previous = nullptr;
    Chunk* p_found = nullptr;
    for (Chunk* p_contiguous_start = p_free; p_contiguous_start != nullptr; p_contiguous_start = p_contiguous_start->p_next) {
        if (p_contiguous_start->contiguous_size >= chunk_count_with_header) {
            p_found = p_contiguous_start;
            break;
        }
        p_previous = p_contiguous_start;
    }

    if (p_found == nullptr) {
        return std::nullopt;
    }

    if (p_found->contiguous_size > chunk_count_with_header) {
        // Shrink contiguous region first
        Chunk* p_remainder = p_found + chunk_count_with_header;
        p_remainder->contiguous_size = p_found->contiguous_size - chunk_count_with_header;
        p_remainder->p_next = p_found->p_next;

        p_found->contiguous_size = chunk_count_with_header;
        p_found->p_next = p_remainder;
    }

    ENSURE_CORRECTNESS();

    // Linked list removal
    Chunk* p_after = p_found->p_next;
    
    if (p_previous == nullptr) {
        // Removing head
        p_free = p_after;
    }
    else {
        p_previous->p_next = p_after;
    }

    ENSURE_CORRECTNESS();

    prepare_reservation(p_found);
    ENSURE_CORRECTNESS();
    return p_found->p_next;
}

bool Allocator::Page::extend_contiguous_from(Chunk* p_from, size_t new_chunk_count) {
    ENSURE_CORRECTNESS();

    Chunk* p_target = p_from + p_from->contiguous_size;
    size_t chunk_count = new_chunk_count - p_target->contiguous_size;

    Chunk* p_previous = nullptr;
    Chunk* p_found = nullptr;
    for (Chunk* p_contiguous_start = p_free; p_contiguous_start != nullptr; p_contiguous_start = p_contiguous_start->p_next) {
        if (p_contiguous_start == p_target) {
            if (p_contiguous_start->contiguous_size >= chunk_count) {
                p_found = p_contiguous_start;
            }
            break;
        }
        else if (p_contiguous_start > p_target) {
            break;
        }
        else {
            p_previous = p_contiguous_start;
        }
    }

    if (p_found == nullptr) {
        return false;
    }

    if (p_found->contiguous_size > chunk_count) {
        // Shrink contiguous region first
        Chunk* p_remainder = p_found + chunk_count;
        p_remainder->contiguous_size = p_found->contiguous_size - chunk_count;
        p_remainder->p_next = p_found->p_next;

        p_found->contiguous_size = chunk_count;
        p_found->p_next = p_remainder;
    }

    ENSURE_CORRECTNESS();

    // Linked list removal
    Chunk* p_after = p_found->p_next;
    
    if (p_previous == nullptr) {
        // Removing head
        p_free = p_after;
    }
    else {
        p_previous->p_next = p_after;
    }
    
    ENSURE_CORRECTNESS();

    merge_reservations(p_from, p_found);
    ENSURE_CORRECTNESS();
    return true;
}

void Allocator::Page::reincorporate_contiguous(Chunk* p_chunk) {
    ENSURE_CORRECTNESS();

    Chunk* p_chunk_end = p_chunk + p_chunk->contiguous_size;

    // Find the chunk most "to the right" that is just before the given chunk
    Chunk* p_previous = nullptr;
    Chunk* p_after = nullptr;
    for (Chunk* p_contiguous_start = p_free; p_contiguous_start != nullptr; p_contiguous_start = p_contiguous_start->p_next) {
        if (p_contiguous_start >= p_chunk) {
            break;
        }
        p_previous = p_contiguous_start;
    }

    close_reservation(p_chunk);
    ENSURE_CORRECTNESS();

    // List insertion

    if (p_previous == nullptr) {
        // Inserting at beginning of list
        p_after = p_free;
        p_chunk->p_next = p_free;
        p_free = p_chunk;
    }
    else {
        p_after = p_previous->p_next;
        if (p_after != nullptr && p_after < p_chunk_end) {
            RCalc::Logger::log_err("Cannot reincorporate contiguous");
            RCalc::Logger::log_err("p_previous: %p : %d", p_previous, p_previous->contiguous_size);
            RCalc::Logger::log_err("p_chunk: %p : %d", p_chunk, p_chunk->contiguous_size);
            RCalc::Logger::log_err("p_chunk_end: %p", p_chunk_end);
            RCalc::Logger::log_err("p_after: %p : %d", p_after, p_after->contiguous_size);

            #ifdef DEBUG_ALLOC
            RCalc::Logger::log_err("");
            for (Chunk* p_dbg = p_previous; p_dbg <= p_chunk_end; ++p_dbg) {
                if (DBG_chunks_reserved.contains(p_dbg)) {
                    RCalc::Logger::log_err("%p: RESERVED", p_dbg);
                }
                else {
                    RCalc::Logger::log_err("%p: free", p_dbg);
                }
            }
            #endif
            throw std::logic_error("Cannot reincorporate contiguous when <previous> <given> <after> is not maintained!");
        }

        p_chunk->p_next = p_after;
        p_previous->p_next = p_chunk;
    }

    ENSURE_CORRECTNESS();

    // Try to merge adjacent contiguous chunks
    if (p_chunk_end == p_after) {
        p_chunk->contiguous_size += p_after->contiguous_size;
        p_chunk->p_next = p_after->p_next;
        p_after->contiguous_size = 0;
        p_after->p_next = 0;
    }

    ENSURE_CORRECTNESS();

    if (p_previous != nullptr && (p_previous + p_previous->contiguous_size) == p_chunk) {
        p_previous->contiguous_size += p_chunk->contiguous_size;
        p_previous->p_next = p_chunk->p_next;
        p_chunk->contiguous_size = 0;
        p_chunk->p_next = 0;
    }

    ENSURE_CORRECTNESS();
}


void Allocator::Page::prepare_reservation(Chunk* p_chunk) {
    if (p_chunk->contiguous_size == 0) { throw std::logic_error("Cannot prepare reservation of size 0!"); }

    size_t bytes = (p_chunk->contiguous_size - 1) * sizeof(Chunk);
    p_chunk->p_next = p_chunk + 1;
    memset(chunk_to_void_addr(p_chunk->p_next), 0, bytes);

    #ifdef DEBUG_ALLOC
    for (size_t index = 0; index < p_chunk->contiguous_size; ++index) {
        DBG_chunks_reserved.insert(p_chunk + index);
    }
    #endif
}

void Allocator::Page::merge_reservations(Chunk* p_first, Chunk* p_second) {
    if (p_first->contiguous_size == 0) { throw std::logic_error("Cannot merge reservation of size 0!"); }
    if (p_second->contiguous_size == 0) { throw std::logic_error("Cannot merge reservation of size 0!"); }
    if (p_first >= p_second) { throw std::logic_error("Cannot merge reservations out of order!"); }

    size_t first_chunk_count = p_first->contiguous_size;
    size_t second_chunk_count = p_second->contiguous_size;

    if (p_first + first_chunk_count != p_second) { throw std::logic_error("Cannot merge reservations that are not adjacent!"); }

    size_t bytes = second_chunk_count * sizeof(Chunk);
    memset(chunk_to_void_addr(p_second), 0, bytes);

    p_first->contiguous_size += second_chunk_count;

    #ifdef DEBUG_ALLOC
    for (size_t index = 0; index < second_chunk_count; ++index) {
        DBG_chunks_reserved.insert(p_second + index);
    }
    #endif
}

void Allocator::Page::shrink_reservation(Chunk* p_chunk, size_t new_chunk_count) {
    if (p_chunk->contiguous_size < new_chunk_count) { throw std::logic_error("Cannot shrink reservation to larger size!"); }
    if (new_chunk_count == 0) { throw std::logic_error("Cannot shrink reservation to less than 1 chunk!"); }
    if (p_chunk->contiguous_size == new_chunk_count) { throw std::logic_error("Cannot shrink reservation to the same size!"); }
    
    Chunk* p_remainder = p_chunk + new_chunk_count;
    p_remainder->contiguous_size = p_chunk->contiguous_size - new_chunk_count;
    p_remainder->p_next = p_remainder + 1;
    reincorporate_contiguous(p_remainder);

    p_chunk->contiguous_size = new_chunk_count;
}

void Allocator::Page::close_reservation(Chunk* p_chunk) {
    if (p_chunk->contiguous_size == 0) { throw std::logic_error("Cannot close reservation of size 0!"); }
    if (p_chunk->p_next != p_chunk + 1) { throw std::logic_error("Cannot close non-header chunk!"); }

    size_t bytes = (p_chunk->contiguous_size - 1) * sizeof(Chunk);
    memset(chunk_to_void_addr(p_chunk->p_next), 0, bytes);
    p_chunk->p_next = nullptr;

    #ifdef DEBUG_ALLOC
    for (size_t index = 0; index < p_chunk->contiguous_size; ++index) {
        if (!DBG_chunks_reserved.contains(p_chunk + index)) {
            throw std::logic_error("Closing reservation that contains an unreserved chunk!");
        }
        DBG_chunks_reserved.erase(p_chunk + index);
    }
    #endif
}

#ifdef DEBUG_ALLOC

void Allocator::ENSURE_CORRECTNESS(std::source_location src_loc) {
    for (Page* p_page = p_page_begin; p_page != nullptr; p_page = p_page->next()) {
        p_page->ENSURE_CORRECTNESS(src_loc);
    }
}

void Allocator::Page::ENSURE_CORRECTNESS(std::source_location src_loc) {
    Chunk* p_chunk_begin = p_data;
    Chunk* p_chunk_end = p_data + chunk_count;

    std::map<Chunk*, Chunk*> seen_chunks;

    for (Chunk* p_contiguous_start = p_free; p_contiguous_start != nullptr; p_contiguous_start = p_contiguous_start->p_next) {
        for (size_t chunk_index = 0; chunk_index < p_contiguous_start->contiguous_size; ++chunk_index) {
            Chunk* p_chunk = p_contiguous_start + chunk_index;

            // No free chunk is in the reserved chunks set
            if (DBG_chunks_reserved.contains(p_chunk)) {
                RCalc::Logger::log_err("(%s:%d) Free chunk at address %p was found in the reserved chunks set!\n\tContiguous start: %p, Contiguous size: %d, Chunk index: %d\n\tPage begin: %p, Page end: %p", src_loc.file_name(), src_loc.line(), p_chunk, p_contiguous_start, p_contiguous_start->contiguous_size, chunk_index, p_chunk_begin, p_chunk_end);
                throw std::logic_error("Free chunk was found in the reserved chunks set!");
            }

            // Every free chunk is within the page boundary
            if (p_chunk < p_chunk_begin) {
                RCalc::Logger::log_err("(%s:%d) Free chunk at address %p was located before the page boundary!\n\tContiguous start: %p, Contiguous size: %d, Chunk index: %d\n\tPage begin: %p, Page end: %p", src_loc.file_name(), src_loc.line(), p_chunk, p_contiguous_start, p_contiguous_start->contiguous_size, chunk_index, p_chunk_begin, p_chunk_end);
                throw std::logic_error("Free chunk was located before the page boundary!");
            }
            if (p_chunk >= p_chunk_end) {
                RCalc::Logger::log_err("(%s:%d) Free chunk at address %p was located after the page boundary!\n\tContiguous start: %p, Contiguous size: %d, Chunk index: %d\n\tPage begin: %p, Page end: %p", src_loc.file_name(), src_loc.line(), p_chunk, p_contiguous_start, p_contiguous_start->contiguous_size, chunk_index, p_chunk_begin, p_chunk_end);
                throw std::logic_error("Free chunk was located after the page boundary!");
            }

            // Each free chunk is seen only once
            if (seen_chunks.contains(p_chunk)) {
                RCalc::Logger::log_err("(%s:%d) Free chunk at address %p was found again after already belonging to contiguous region %p!\n\tContiguous start: %p, Contiguous size: %d, Chunk index: %d\n\tPage begin: %p, Page end: %p", src_loc.file_name(), src_loc.line(), p_chunk, seen_chunks[p_chunk], p_contiguous_start, p_contiguous_start->contiguous_size, chunk_index, p_chunk_begin, p_chunk_end);
                throw std::logic_error("Free chunk was found again after already belonging to another contiguous region!");
            }
            seen_chunks[p_chunk] = p_contiguous_start;
        }

        // Free contiguous chunks are strictly increasing
        if (p_contiguous_start->p_next != nullptr && p_contiguous_start->p_next <= p_contiguous_start) {
                RCalc::Logger::log_err("(%s:%d) Free contiguous chunk has a next pointer that does not strictly increase!\n\tContiguous start: %p, Contiguous size: %d, Next Contiguous: %p\n\tPage begin: %p, Page end: %p", src_loc.file_name(), src_loc.line(), p_contiguous_start, p_contiguous_start->contiguous_size, p_contiguous_start->p_next, p_chunk_begin, p_chunk_end);
                throw std::logic_error("Free contiguous chunk next pointer that does not strictly increase!");
        }
    }
}

#endif


Allocator Allocator::shared;
