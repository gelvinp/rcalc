#pragma once

#include "allocator.h"

#include <bit>
#include <cstring>


template<typename T>
class CowVec;

template<typename T>
void swap(CowVec<T>& a, CowVec<T>& b);


template<typename T>
class CowVec {
public:
    CowVec() {}
    CowVec(std::initializer_list<T> list) {
        reserve(list.size());
        for (const T& element : list) {
            push_back(element);
        }
    }
    CowVec(const CowVec& other) {
        p_data = other.p_data;
        if (p_data) {
            p_data->refcount++;
        }
    }
    CowVec(CowVec&& other) : CowVec() {
        swap(*this, other);
    }
    ~CowVec() {
        unref();
    }

    CowVec& operator=(CowVec other) {
        swap(*this, other);
        return *this;
    }
    CowVec& operator=(CowVec&& other) {
        swap(*this, other);
        return *this;
    }


    size_t size() const {
        if (p_data == nullptr) { return 0;}
        return p_data->size;
    }

    size_t capacity() const {
        if (p_data == nullptr) { return 0;}
        size_t alloc_size = Allocator::get_allocation_size(reinterpret_cast<void*>(p_data));
        return allocation_size_to_capacity(alloc_size);
    }

    bool empty() const { return size() == 0; }


    bool same_ref(const CowVec& other) const { return p_data == other.p_data; }


    void reserve(size_t new_capacity) {
        ensure_unique();
        size_t new_size_bytes = capacity_to_allocation_size(new_capacity);

        if (p_data == nullptr) {
            // First allocation
            p_data = reinterpret_cast<CowData*>(Allocator::alloc(new_size_bytes));
            p_data->refcount = 1;
            p_data->size = 0;
        }
        else {
            size_t current_capacity = capacity();
            if (current_capacity >= new_capacity) { return; }
            p_data = reinterpret_cast<CowData*>(Allocator::realloc(reinterpret_cast<void*>(p_data), new_size_bytes));
        }

        #ifndef NDEBUG
        if (capacity() < new_capacity) {
            RCalc::Logger::log_err("Capacity after reserve: %d is not what was expected: %d", capacity(), new_capacity);
            throw std::logic_error("Capacity after reserve is not what was expected!");
        }
        #endif
    }

    void resize(size_t new_size, T new_element = T()) {
        ensure_unique();
        size_t current_size = size();

        if (new_size == current_size) { return; }
        if (new_size == 0) {
            unref();
            return;
        }

        if (new_size > current_size) {
            // Grow
            reserve(new_size);
            for (size_t index = current_size; index < new_size; ++index) {
                push_back(new_element);
            }
        }
        else {
            // Shrink
            if (!std::is_trivially_destructible<T>::value) {
                for (size_t index = current_size - 1; index >= new_size; --index) {
                    p_data->ptr[index].~T();
                }
            }

            size_t new_size_bytes = capacity_to_allocation_size(new_size);
            p_data = reinterpret_cast<CowData*>(Allocator::realloc(reinterpret_cast<void*>(p_data), new_size_bytes));

            #ifndef NDEBUG
            if (capacity() < new_size) {
                RCalc::Logger::log_err("Capacity after shrink %d is not what was expected: %d!", capacity(), new_size);
                throw std::logic_error("Capacity after resize (shrink) is not what was expected!");
            }
            #endif
        }

        p_data->size = new_size;
    }

    void clear() { resize(0); }


    void push_back(const T& value) {
        ensure_unique();
        ensure_capacity_for_new_element();
        p_data->ptr[p_data->size++] = value;
    }

    void push_back(T&& value) {
        ensure_unique();
        ensure_capacity_for_new_element();
        p_data->ptr[p_data->size++] = std::move(value);
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        ensure_unique();
        ensure_capacity_for_new_element();
        return *new(&p_data->ptr[p_data->size++]) T(args...);
    }


    const T& at(size_t index) const {
        #ifndef NDEBUG
        if (index >= size()) {
            RCalc::Logger::log_err("CowVec index %d is out of bounds (size: %d)", index, size());
            throw std::logic_error("CowVec index out of bounds!");
        }
        #endif
        return p_data->ptr[index];
    }

    T& mut_at(size_t index) {
        ensure_unique();
        #ifndef NDEBUG
        if (index >= size()) {
            RCalc::Logger::log_err("CowVec index %d is out of bounds (size: %d)", index, size());
            throw std::logic_error("CowVec index out of bounds!");
        }
        #endif
        return p_data->ptr[index];
    }


    const T& operator[](size_t index) const {
        return at(index);
    }
        
    struct Iterator {
        using iterator_category = std::contiguous_iterator_tag;
        using difference_type = std::size_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using element_type = T;

        Iterator(pointer ptr = nullptr) : ptr(ptr) {}

        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }

        Iterator& operator++() { ptr++; return *this; }
        Iterator operator++(int) { Iterator prev = *this; ptr++; return prev; }

        Iterator& operator--() { ptr--; return *this; }
        Iterator operator--(int) { Iterator prev = *this; ptr--; return prev; }

        Iterator& operator+=(difference_type dist) { ptr += dist; return *this; }
        Iterator& operator-=(difference_type dist) { ptr -= dist; return *this; }

        auto operator<=>(const Iterator&) const = default;

        reference operator[](size_t index) const { return *(ptr + index); }

        friend bool operator==(const Iterator& a, const Iterator& b) { return a.ptr == b.ptr; }

        friend Iterator operator+(Iterator a, difference_type b) { return a += b; }
        friend Iterator operator-(Iterator a, difference_type b) { return a -= b; }

        friend Iterator operator+(difference_type b, Iterator a) { return a += b; }
        friend Iterator operator-(difference_type b, Iterator a) { return a -= b; }

        friend difference_type operator-(Iterator a, const Iterator& b) { return b.ptr - a.ptr; }

    private:
        pointer ptr = nullptr;
    };


    struct ConstIterator {
        using iterator_category = std::contiguous_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using element_type = const T;

        ConstIterator(pointer ptr = nullptr) : ptr(ptr) {}

        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }

        ConstIterator& operator++() { ptr++; return *this; }
        ConstIterator operator++(int) { ConstIterator prev = *this; ptr++; return prev; }

        ConstIterator& operator--() { ptr--; return *this; }
        ConstIterator operator--(int) { ConstIterator prev = *this; ptr--; return prev; }

        ConstIterator& operator+=(difference_type dist) { ptr += dist; return *this; }
        ConstIterator& operator-=(difference_type dist) { ptr -= dist; return *this; }

        auto operator<=>(const ConstIterator&) const = default;

        reference operator[](size_t index) const { return *(ptr + index); }

        friend bool operator==(const ConstIterator& a, const ConstIterator& b) { return a.ptr == b.ptr; }

        friend ConstIterator operator+(ConstIterator a, difference_type b) { return a += b; }
        friend ConstIterator operator-(ConstIterator a, difference_type b) { return a -= b; }

        friend ConstIterator operator+(difference_type b, ConstIterator a) { return a += b; }
        friend ConstIterator operator-(difference_type b, ConstIterator a) { return a -= b; }

        friend difference_type operator-(ConstIterator a, const ConstIterator& b) { return b.ptr - a.ptr; }

    private:
        pointer ptr = nullptr;
    };


    Iterator mut_begin() {
        ensure_unique();
        if (p_data == nullptr) {
            return Iterator(nullptr);
        }
        return Iterator(p_data->ptr);
    }

    Iterator mut_end() {
        ensure_unique();
        if (p_data == nullptr) {
            return Iterator(nullptr);
        }
        return Iterator(p_data->ptr + p_data->size);
    }


    ConstIterator begin() const {
        if (p_data == nullptr) {
            return ConstIterator(nullptr);
        }
        return ConstIterator(p_data->ptr);
    }

    ConstIterator end() const {
        if (p_data == nullptr) {
            return ConstIterator(nullptr);
        }
        return ConstIterator(p_data->ptr + p_data->size);
    }


    const T& front() const {
        if (p_data == nullptr) {
            throw std::logic_error("Cannot call front on an empty CowVec!");
        }
        return p_data->ptr[0];
    }

    T& mut_front() {
        ensure_unique();
        if (p_data == nullptr) {
            throw std::logic_error("Cannot call front on an empty CowVec!");
        }
        return p_data->ptr[0];
    }


    const T& back() const {
        if (p_data == nullptr) {
            throw std::logic_error("Cannot call back on an empty CowVec!");
        }
        return p_data->ptr[p_data->size - 1];
    }

    T& mut_back() {
        ensure_unique();
        if (p_data == nullptr) {
            throw std::logic_error("Cannot call back on an empty CowVec!");
        }
        return p_data->ptr[p_data->size - 1];
    }

private:
    struct CowData {
        size_t refcount;
        size_t size;
        T ptr[1];
    };

    CowData* p_data = nullptr;

    void ensure_unique() {
        if (p_data == nullptr) { return; }
        if (p_data->refcount == 1) { return; }

        size_t current_bytes = Allocator::get_allocation_size(reinterpret_cast<void*>(p_data));

        CowData* p_new_data = reinterpret_cast<CowData*>(Allocator::alloc(current_bytes));
        p_new_data->refcount = 1;
        p_new_data->size = p_data->size;

        copy_elements(p_new_data);

        unref();
        p_data = p_new_data;
    }

    template<typename R = T, std::enable_if_t<std::is_trivially_copy_assignable<R>::value, bool> = true>
    void copy_elements(CowData* p_new_data) {
        memcpy(p_new_data->ptr, p_data->ptr, p_data->size * sizeof(R));
    }

    template<typename R = T, std::enable_if_t<!std::is_trivially_copy_assignable<R>::value, bool> = true>
    void copy_elements(CowData* p_new_data) {
        for (size_t index = 0; index < p_data->size; ++index) {
            new(&p_new_data->ptr[index]) R(p_data->ptr[index]);
        }
    }

    void ensure_capacity_for_new_element() {
        size_t cap = capacity();
        if (size() + 1 <= cap) { return; }

        size_t next_cap;
        
        if (cap < 4) {
            next_cap = 4;
        }
        else {
            next_cap = std::bit_ceil(cap + 1);
        }

        reserve(next_cap);
    }

    void unref() {
        if (p_data == nullptr) { return; }
        if (--p_data->refcount > 0) { return; }

        if (!std::is_trivially_destructible<T>::value) {
            for (size_t index = 0; index < p_data->size; ++index) {
                p_data->ptr[index].~T();
            }
        }

        Allocator::free(reinterpret_cast<void*>(p_data));
        p_data = nullptr;
    }


    size_t capacity_to_allocation_size(size_t count) const {
        return (2 * sizeof(size_t)) + (count * sizeof(T));
    }

    size_t allocation_size_to_capacity(size_t size_bytes) const {
        return (size_bytes - (2 * sizeof(size_t))) / sizeof(T);
    }

    friend void swap<T>(CowVec& a, CowVec& b);
};


template<typename T>
void swap(CowVec<T>& a, CowVec<T>& b) {
    using namespace std;
    swap(a.p_data, b.p_data);
};