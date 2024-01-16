#include "snitch/snitch.hpp"
#include "core/memory/allocator.h"

#include <random>

using namespace RCalc;

TEST_CASE("Must be ready", "[core][alloc][allocates]") {
    Allocator::cleanup();

    SECTION("to realloc") {
        int a;
        REQUIRE_THROWS_AS(Allocator::realloc((void*)&a, 1234), std::logic_error);
    }
    SECTION("to free") {
        int a;
        REQUIRE_THROWS_AS(Allocator::free((void*)&a), std::logic_error);
    }
    SECTION("to get allocation size") {
        int a;
        REQUIRE_THROWS_AS(Allocator::get_allocation_size((void*)&a), std::logic_error);
    }

    SECTION("not to alloc") {
        void* p_addr = Allocator::alloc(sizeof(Allocator::Page::Chunk));
        REQUIRE(Allocator::get_allocation_size(p_addr) == sizeof(Allocator::Page::Chunk));
        Allocator::free(p_addr);
    }
}

TEST_CASE("Creates a new page when not enough room", "[core][alloc][allocates]") {
    Allocator::cleanup();

    constexpr const size_t size = Allocator::DEFAULT_PAGE_SIZE + 1024;

    void* p_addr = Allocator::alloc(size);
    REQUIRE(Allocator::get_allocation_size(p_addr) == size);
    Allocator::free(p_addr);
}

TEST_CASE("Grows a reservation in place when able", "[core][alloc][allocates]") {
    Allocator::cleanup();

    constexpr const size_t initial_size = 1024;
    constexpr const size_t expanded_size = 2048;

    static_assert(expanded_size < Allocator::DEFAULT_PAGE_SIZE);
    static_assert(initial_size < expanded_size);

    void* p_initial_addr = Allocator::alloc(initial_size);
    REQUIRE(Allocator::get_allocation_size(p_initial_addr) == initial_size);

    void* p_expanded_addr = Allocator::realloc(p_initial_addr, expanded_size);
    REQUIRE(Allocator::get_allocation_size(p_expanded_addr) == expanded_size);

    REQUIRE(p_initial_addr == p_expanded_addr);

    Allocator::free(p_expanded_addr);
}

TEST_CASE("Moves a reservation when unable to grow", "[core][alloc][allocates]") {
    Allocator::cleanup();

    constexpr const size_t count = 1024;

    constexpr const size_t initial_size = count * sizeof(size_t);
    constexpr const size_t expanded_size = Allocator::DEFAULT_PAGE_SIZE + initial_size;

    static_assert(expanded_size > Allocator::DEFAULT_PAGE_SIZE);
    static_assert(initial_size < Allocator::DEFAULT_PAGE_SIZE);

    void* p_initial_addr = Allocator::alloc(initial_size);
    REQUIRE(Allocator::get_allocation_size(p_initial_addr) == initial_size);

    size_t* p_arr = reinterpret_cast<size_t*>(p_initial_addr);

    for (size_t index = 0; index < count; ++index) {
        p_arr[index] = index;
    }

    void* p_expanded_addr = Allocator::realloc(p_initial_addr, expanded_size);
    REQUIRE(Allocator::get_allocation_size(p_expanded_addr) == expanded_size);

    REQUIRE(p_initial_addr != p_expanded_addr);

    p_arr = reinterpret_cast<size_t*>(p_expanded_addr);

    for (size_t index = 0; index < count; ++index) {
        REQUIRE(p_arr[index] == index);
    }

    REQUIRE(p_arr[count] == 0);

    Allocator::free(p_expanded_addr);
}

TEST_CASE("Shrinks a reservation in place", "[core][alloc][allocates]") {
    Allocator::cleanup();

    constexpr const size_t initial_size = 2048;
    constexpr const size_t shrunk_size = 1024;

    static_assert(initial_size < Allocator::DEFAULT_PAGE_SIZE);
    static_assert(shrunk_size < initial_size);

    void* p_initial_addr = Allocator::alloc(initial_size);
    REQUIRE(Allocator::get_allocation_size(p_initial_addr) == initial_size);

    void* p_shrunk_addr = Allocator::realloc(p_initial_addr, shrunk_size);
    REQUIRE(Allocator::get_allocation_size(p_shrunk_addr) == shrunk_size);

    REQUIRE(p_initial_addr == p_shrunk_addr);

    Allocator::free(p_shrunk_addr);
}

TEST_CASE("Allocation Stress Test", "[core][alloc][allocates][.long]") {
    Allocator::cleanup();
    Allocator::setup();

    constexpr size_t ALLOC_COUNT = 1024;
    uint8_t* p_alloc_arr[ALLOC_COUNT] = {};
    size_t alloc_size_arr[ALLOC_COUNT] = {};

    std::random_device rnd;
    std::default_random_engine eng { rnd() };
    std::uniform_int_distribution<size_t> dist { 0, 1000000 };

    for (size_t index = 0; index < ALLOC_COUNT; ++index) {
        alloc_size_arr[index] = dist(eng);
        CAPTURE(alloc_size_arr[index]);
        p_alloc_arr[index] = reinterpret_cast<uint8_t*>(Allocator::alloc(alloc_size_arr[index]));
        REQUIRE(alloc_size_arr[index] <= Allocator::get_allocation_size(reinterpret_cast<void*>(p_alloc_arr[index])));
        REQUIRE((Allocator::get_allocation_size(reinterpret_cast<void*>(p_alloc_arr[index])) - alloc_size_arr[index]) < sizeof(Allocator::Page::Chunk));
    }

    auto require_no_overlap = [](uint8_t* a_start, size_t a_size, uint8_t* b_start, size_t b_size) {
        uint8_t* a_end = a_start + a_size;
        uint8_t* b_end = b_start + b_size;

        REQUIRE((a_end <= b_start) || (b_end <= a_start));
    };

    for (size_t check_index = 0; check_index < ALLOC_COUNT; ++check_index) {
        for (size_t against_index = check_index + 1; against_index < ALLOC_COUNT; ++against_index) {
            require_no_overlap(p_alloc_arr[check_index], alloc_size_arr[check_index], p_alloc_arr[against_index], alloc_size_arr[against_index]);
        }
    }

    constexpr size_t ITER_COUNT = ALLOC_COUNT * 100;
    std::uniform_int_distribution<size_t> index_dist { 0, ALLOC_COUNT - 1 };

    for (size_t _i = 0; _i < ITER_COUNT; ++_i) {
        size_t index = index_dist(eng);
        size_t command = 0;

        if (p_alloc_arr[index] != nullptr) {
            command = (dist(eng) % 2) + 1;
        }

        if (command == 0) {
            // Allocate
            alloc_size_arr[index] = dist(eng);
            CAPTURE(alloc_size_arr[index]);
            p_alloc_arr[index] = reinterpret_cast<uint8_t*>(Allocator::alloc(alloc_size_arr[index]));
            REQUIRE(alloc_size_arr[index] <= Allocator::get_allocation_size(reinterpret_cast<void*>(p_alloc_arr[index])));
            REQUIRE((Allocator::get_allocation_size(reinterpret_cast<void*>(p_alloc_arr[index])) - alloc_size_arr[index]) < sizeof(Allocator::Page::Chunk));
        }
        else if (command == 1) {
            // Reallocate
            alloc_size_arr[index] = dist(eng);
            CAPTURE(alloc_size_arr[index]);
            p_alloc_arr[index] = reinterpret_cast<uint8_t*>(Allocator::realloc(reinterpret_cast<void*>(p_alloc_arr[index]), alloc_size_arr[index]));
            REQUIRE(alloc_size_arr[index] <= Allocator::get_allocation_size(reinterpret_cast<void*>(p_alloc_arr[index])));
            REQUIRE((Allocator::get_allocation_size(reinterpret_cast<void*>(p_alloc_arr[index])) - alloc_size_arr[index]) < sizeof(Allocator::Page::Chunk));
        }
        else {
            // Free
            Allocator::free(reinterpret_cast<void*>(p_alloc_arr[index]));
            p_alloc_arr[index] = nullptr;
        }
    }

    for (size_t index = 0; index < ALLOC_COUNT; ++index) {
        if (p_alloc_arr[index] == nullptr) { continue; }
        Allocator::free(reinterpret_cast<void*>(p_alloc_arr[index]));
    }
}

namespace {
struct ComplexClass {
    static size_t construct_count;
    static size_t destruct_count;

    uint8_t data[sizeof(Allocator::Page::Chunk) * 4];

    ComplexClass() { ++construct_count; }
    ~ComplexClass() { ++destruct_count; }
};

size_t ComplexClass::construct_count = 0;
size_t ComplexClass::destruct_count = 0;


TEST_CASE("std::shared_ptr deleters", "[core][alloc][allocates]") {
    Allocator::cleanup();
    Allocator::setup();
    ComplexClass::construct_count = 0;
    ComplexClass::destruct_count = 0;

    size_t initial = Allocator::shared.p_page_begin->p_free->contiguous_size;
    CAPTURE(initial);

    {
        std::shared_ptr<ComplexClass> ptr = Allocator::make_shared<ComplexClass>();
        size_t during = Allocator::shared.p_page_begin->p_free->contiguous_size;
        CAPTURE(during);
        REQUIRE(during == (initial - 5)); // Four chunks of data + 1 chunk of header
        REQUIRE(ComplexClass::construct_count == 1);
        REQUIRE(ComplexClass::destruct_count == 0);
    }

    REQUIRE(ComplexClass::construct_count == 1);
    REQUIRE(ComplexClass::destruct_count == 1);

    size_t after = Allocator::shared.p_page_begin->p_free->contiguous_size;
    CAPTURE(after);
    REQUIRE(after == initial);
}


TEST_CASE("std::shared_ptr deleters", "[core][alloc][allocates]") {
    Allocator::cleanup();
    Allocator::setup();
    ComplexClass::construct_count = 0;
    ComplexClass::destruct_count = 0;

    size_t initial = Allocator::shared.p_page_begin->p_free->contiguous_size;
    CAPTURE(initial);

    ComplexClass* ptr = Allocator::create<ComplexClass>();
    size_t during = Allocator::shared.p_page_begin->p_free->contiguous_size;
    CAPTURE(during);
    REQUIRE(during == (initial - 5)); // Four chunks of data + 1 chunk of header
    REQUIRE(ComplexClass::construct_count == 1);
    REQUIRE(ComplexClass::destruct_count == 0);

    Allocator::destroy(ptr);

    REQUIRE(ComplexClass::construct_count == 1);
    REQUIRE(ComplexClass::destruct_count == 1);

    size_t after = Allocator::shared.p_page_begin->p_free->contiguous_size;
    CAPTURE(after);
    REQUIRE(after == initial);
}

}