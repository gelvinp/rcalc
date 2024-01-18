#include "snitch/snitch.hpp"
#include "core/memory/cowvec.h"

using namespace RCalc;

TEST_CASE("Initial size and capacity are zero", "[core][cowvec][allocates]") {
    CowVec<int> vec;

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 0);
    REQUIRE(vec.empty());
    REQUIRE(vec.begin() == vec.end());
    REQUIRE(vec.mut_begin() == vec.mut_end());
    #ifndef NDEBUG
    REQUIRE_THROWS_AS(vec.at(1), std::logic_error);
    REQUIRE_THROWS_AS(vec.mut_at(1), std::logic_error);
    REQUIRE_THROWS_AS(vec.front(), std::logic_error);
    REQUIRE_THROWS_AS(vec.back(), std::logic_error);
    REQUIRE_THROWS_AS(vec.mut_front(), std::logic_error);
    REQUIRE_THROWS_AS(vec.mut_back(), std::logic_error);
    #endif
}

TEST_CASE("Can push back", "[core][cowvec][allocates]") {
    CowVec<int> vec;

    int a = 3;
    vec.push_back(a);
    vec.push_back(5);

    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() == vec.INITIAL_CAPACITY);
    REQUIRE_FALSE(vec.empty());
}

TEST_CASE("Automatically resizes when needed", "[core][cowvec][allocates]") {
    CowVec<int> vec;

    for (size_t index = 0; index < vec.INITIAL_CAPACITY; ++index) {
        vec.push_back(index);

        int check_count = 0;
        for (const int& check : vec) {
            REQUIRE(check == check_count);
            REQUIRE(check == vec[check_count]);
            REQUIRE(check == vec.mut_at(check_count));
            check_count++;
        }
        REQUIRE(check_count == index + 1);
    }

    REQUIRE(vec.size() == vec.INITIAL_CAPACITY);
    REQUIRE(vec.capacity() == vec.INITIAL_CAPACITY);

    vec.push_back(vec.INITIAL_CAPACITY);

    REQUIRE(vec.size() == vec.INITIAL_CAPACITY + 1);
    REQUIRE(vec.capacity() == std::bit_ceil(vec.INITIAL_CAPACITY + 1));

    int check_count = 0;
    for (const int& check : vec) {
        REQUIRE(check == check_count);
        REQUIRE(check == vec[check_count]);
        REQUIRE(check == vec.mut_at(check_count));
        check_count++;
    }
    REQUIRE(check_count == vec.INITIAL_CAPACITY + 1);

    REQUIRE(vec.front() == 0);
    REQUIRE(vec.mut_front() == 0);
    REQUIRE(vec.back() == vec.INITIAL_CAPACITY);
    REQUIRE(vec.mut_back() == vec.INITIAL_CAPACITY);
}

TEST_CASE("Can manually resize", "[core][cowvec][allocates]") {
    CowVec<int> vec;

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 0);

    vec.reserve(4);

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 4);

    vec.reserve(2);

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 4);

    vec.resize(8, 42);

    REQUIRE(vec.size() == 8);
    REQUIRE(vec.capacity() == 8);

    for (const int& dut : vec) {
        REQUIRE(dut == 42);
    }

    vec.resize(4);

    REQUIRE(vec.size() == 4);
    REQUIRE(vec.capacity() == 4);

    for (const int& dut : vec) {
        REQUIRE(dut == 42);
    }

    vec.clear();

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 0);
    REQUIRE(vec.p_data == nullptr);
}

TEST_CASE("Performs copy on write", "[core][cowvec][allocates]") {
    CowVec<int> vec_a { 1, 2, 3, 4 };
    CowVec<int> vec_b { vec_a };

    REQUIRE(vec_a.same_ref(vec_b));

    vec_a.size();
    vec_a.capacity();
    vec_a.empty();
    vec_a.at(0);
    vec_a[1];
    vec_a.begin();
    vec_a.end();
    vec_a.front();
    vec_a.back();

    vec_a.reserve(2);
    REQUIRE_FALSE(vec_a.same_ref(vec_b));

    REQUIRE(vec_a.size() == vec_b.size());
    for (size_t index = 0; index < vec_a.size(); ++index) {
        REQUIRE(vec_a[index] == vec_b[index]);
    }

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    vec_a.resize(5, 5);
    REQUIRE_FALSE(vec_a.same_ref(vec_b));

    for (int index = 0; index < vec_a.size(); ++index) {
        REQUIRE(vec_a[index] == index + 1);
    }

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    vec_a.push_back(6);
    REQUIRE_FALSE(vec_a.same_ref(vec_b));

    for (int index = 0; index < vec_a.size(); ++index) {
        REQUIRE(vec_a[index] == index + 1);
    }

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    int seven = 7;
    vec_a.push_back(seven);
    REQUIRE_FALSE(vec_a.same_ref(vec_b));

    for (int index = 0; index < vec_a.size(); ++index) {
        REQUIRE(vec_a[index] == index + 1);
    }

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    vec_a.mut_at(0) = 8;
    REQUIRE_FALSE(vec_a.same_ref(vec_b));
    REQUIRE(vec_a[0] == 8);
    REQUIRE(vec_b[0] == 1);

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    vec_a.mut_front() = 4;
    REQUIRE_FALSE(vec_a.same_ref(vec_b));
    REQUIRE(vec_a[0] == 4);
    REQUIRE(vec_b[0] == 8);

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    vec_a.mut_back() = 4;
    REQUIRE_FALSE(vec_a.same_ref(vec_b));
    REQUIRE(vec_a[vec_a.size() - 1] == 4);
    REQUIRE(vec_b[vec_b.size() - 1] == vec_b.size());

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    int value = vec_a.size();
    for (auto it = vec_a.mut_begin(); it != vec_a.mut_end(); ++it) {
        *it = value--;
    }

    REQUIRE_FALSE(vec_a.same_ref(vec_b));
    
    value = vec_a.size();
    for (const int& dut : vec_a) {
        REQUIRE(dut == value);
        value--;
    }

    vec_b = vec_a;
    REQUIRE(vec_a.same_ref(vec_b));

    REQUIRE(vec_a.p_data->refcount == 2);
    vec_b.unref();

    REQUIRE_FALSE(vec_a.same_ref(vec_b));
    REQUIRE(vec_a.p_data->refcount == 1);

    REQUIRE(vec_b.p_data == nullptr);

    CowVec<int>::CowData* p_data = vec_a.p_data;

    vec_b = CowVec<int> { std::move(vec_a) };

    REQUIRE(vec_a.p_data == nullptr);
    REQUIRE(vec_b.p_data == p_data);

    swap(vec_a, vec_b);

    REQUIRE(vec_a.p_data == p_data);
    REQUIRE(vec_b.p_data == nullptr);
}

TEST_CASE("Iterators behave", "[core][cowvec][allocates]") {
    CowVec<int> vec { 1, 2, 3, 4 };
    CowVec<int>::Iterator it = vec.mut_begin();

    REQUIRE(*it == 1);
    REQUIRE(*(it.operator ->() + 1) == 2);

    REQUIRE(*(++it) == 2);
    REQUIRE(*(it++) == 2);
    REQUIRE(*it == 3);

    REQUIRE(*(--it) == 2);
    REQUIRE(*(it--) == 2);
    REQUIRE(*it == 1);

    REQUIRE(*(it += 2) == 3);
    REQUIRE(*it == 3);

    REQUIRE(*(it - 2) == 1);
    REQUIRE(*(-2 + it) == 1);
    REQUIRE(*it == 3);

    REQUIRE(*(it -= 2) == 1);
    REQUIRE(*it == 1);

    REQUIRE(*(it + 2) == 3);
    REQUIRE(*(2 + it) == 3);
    REQUIRE(*it == 1);

    REQUIRE(it < (it + 2));
    REQUIRE((it + 2) == (it + 2));
    REQUIRE((it + 2) > it);

    REQUIRE(it[0] == 1);
    REQUIRE(it[1] == 2);
    REQUIRE(it[2] == 3);
    REQUIRE(it[3] == 4);

    it[0] = 4;
    it[1] = 3;
    it[2] = 2;
    it[3] = 1;

    REQUIRE(it[0] == 4);
    REQUIRE(it[1] == 3);
    REQUIRE(it[2] == 2);
    REQUIRE(it[3] == 1);

    REQUIRE((vec.mut_end() - vec.mut_begin()) == vec.size());
}

TEST_CASE("ConstIterators behave", "[core][cowvec][allocates]") {
    CowVec<int> vec { 1, 2, 3, 4 };
    CowVec<int>::ConstIterator it = vec.begin();

    REQUIRE(*it == 1);
    REQUIRE(*(it.operator ->() + 1) == 2);

    REQUIRE(*(++it) == 2);
    REQUIRE(*(it++) == 2);
    REQUIRE(*it == 3);

    REQUIRE(*(--it) == 2);
    REQUIRE(*(it--) == 2);
    REQUIRE(*it == 1);

    REQUIRE(*(it += 2) == 3);
    REQUIRE(*it == 3);

    REQUIRE(*(it - 2) == 1);
    REQUIRE(*(-2 + it) == 1);
    REQUIRE(*it == 3);

    REQUIRE(*(it -= 2) == 1);
    REQUIRE(*it == 1);

    REQUIRE(*(it + 2) == 3);
    REQUIRE(*(2 + it) == 3);
    REQUIRE(*it == 1);

    REQUIRE(it < (it + 2));
    REQUIRE((it + 2) == (it + 2));
    REQUIRE((it + 2) > it);

    REQUIRE(it[0] == 1);
    REQUIRE(it[1] == 2);
    REQUIRE(it[2] == 3);
    REQUIRE(it[3] == 4);

    REQUIRE((vec.end() - vec.begin()) == vec.size());
}

namespace {
    
struct ComplexClass {
    static size_t copy_count;
    static size_t assign_count;
    static size_t destruct_count;

    ComplexClass() {}
    ComplexClass(const ComplexClass& other) { ++copy_count; }
    ComplexClass& operator=(const ComplexClass& other) { ++assign_count; return *this; }
    ~ComplexClass() { ++destruct_count; }
};

size_t ComplexClass::copy_count = 0;
size_t ComplexClass::assign_count = 0;
size_t ComplexClass::destruct_count = 0;

TEST_CASE("Handles non-trivial classes", "[core][cowvec][allocates]") {
    ComplexClass::copy_count = 0;
    ComplexClass::assign_count = 0;
    ComplexClass::destruct_count = 0;

    CowVec<ComplexClass> vec_a;
    vec_a.resize(16);

    REQUIRE(ComplexClass::assign_count == 16);

    CowVec<ComplexClass> vec_b { vec_a };

    vec_a.resize(14);
    REQUIRE(ComplexClass::copy_count == 16);
    REQUIRE(ComplexClass::destruct_count == 4);
}

}