#include "value.h"

#include "core/logger.h"

#include <algorithm>
#include <charconv>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <typeinfo>

namespace RCalc {

#pragma region pool

#pragma region pool_macro

#ifndef NDEBUG

#define DEFINE_POOL(type) \
class Pool_##type { \
public: \
    static uint64_t allocate(type value) { \
        if (_pool.size() != _free.size()) { \
            Logger::log_err("Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _free.size()); \
            throw std::logic_error("Pool size mismatch, check log output for more information."); \
        } \
\
        std::vector<bool>::iterator it = std::find(_free.begin(), _free.end(), false); \
        if (it != _free.end()) { \
            /* Reuse slot */ \
            *it = true; \
            uint64_t index = static_cast<uint64_t>(it - _free.begin()); \
            _pool[index] = value; \
            return static_cast<uint64_t>(it - _free.begin()); \
        } \
\
        if (_pool.size() > std::numeric_limits<uint64_t>::max()) { \
            Logger::log_err("Cannot allocate any more Values of type '" #type "'!"); \
            throw std::overflow_error("Value pool full, check log output for more information."); \
        } \
\
        /* New slot */ \
        _pool.push_back(value); \
        _free.push_back(true); \
\
        return static_cast<uint64_t>(_pool.size() - 1); \
    } \
\
    static type get(uint64_t index) { \
        if (_pool.size() != _free.size()) { \
            Logger::log_err("Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _free.size()); \
            throw std::logic_error("Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("Pool access out of bounds for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("Pool free out of bounds, check log output for more information."); \
        } \
        if (_free[index] == false) { \
            Logger::log_err("Pool use after free for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("Pool use after free, check log output for more information."); \
        } \
\
        return _pool[index]; \
    } \
\
    static void free(uint64_t index) { \
        if (_pool.size() != _free.size()) { \
            Logger::log_err("Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _free.size()); \
            throw std::logic_error("Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("Pool free out of bounds for type '" #type "'!\n\tPool size: %d\tFreed index: %d", _pool.size(), index); \
            throw std::logic_error("Pool free out of bounds, check log output for more information."); \
        } \
        if (_free[index] == false) { \
            Logger::log_err("Pool double free for type '" #type "'!\n\tPool size: %d\tFreed index: %d", _pool.size(), index); \
            throw std::logic_error("Pool double free, check log output for more information."); \
        } \
\
        _free[index] = false; \
\
        /* Attempt to shrink */ \
        std::vector<bool>::const_iterator it = std::find(_free.rbegin(), _free.rend(), true).base(); \
        if (it == _free.end()) { return; } \
        size_t new_size = it - _free.begin(); \
\
        for (auto itd = it; itd < _free.end(); ++itd) { \
            if (*itd) { \
                Logger::log_err("Pool shrink non-free value for type '" #type "'!\n\tPool size: %d\tFreed index: %d", _pool.size(), itd - _free.begin()); \
            } \
        } \
\
        _pool.resize(new_size); \
        _free.resize(new_size); \
    } \
\
private: \
    static std::vector<type> _pool; \
    static std::vector<bool> _free; \
}; \
\
std::vector<type> Pool_##type::_pool; \
std::vector<bool> Pool_##type::_free;

#else

#define DEFINE_POOL(type) \
class Pool_##type { \
public: \
    static uint64_t allocate(type value) { \
        std::vector<bool>::iterator it = std::find(_free.begin(), _free.end(), false); \
        if (it != _free.end()) { \
            /* Reuse slot */ \
            *it = true; \
            uint64_t index = static_cast<uint64_t>(it - _free.begin()); \
            _pool[index] = value; \
            return static_cast<uint64_t>(it - _free.begin()); \
        } \
\
        if (_pool.size() > std::numeric_limits<uint64_t>::max()) { \
            Logger::log_err("Cannot allocate any more Values of type '" #type "'!"); \
            throw std::overflow_error("Value pool full, check log output for more information."); \
        } \
\
        /* New slot */ \
        _pool.push_back(value); \
        _free.push_back(true); \
\
        return static_cast<uint64_t>(_pool.size() - 1); \
    } \
\
    static type get(uint64_t index) { \
        return _pool[index]; \
    } \
\
    static void free(uint64_t index) { \
        _free[index] = false; \
\
        /* Attempt to shrink */ \
        std::vector<bool>::const_iterator it = std::find(_free.rbegin(), _free.rend(), true).base(); \
        if (it == _free.end()) { return; } \
        size_t new_size = it - _free.begin(); \
\
        _pool.resize(new_size); \
        _free.resize(new_size); \
    } \
\
private: \
    static std::vector<type> _pool; \
    static std::vector<bool> _free; \
}; \
\
std::vector<type> Pool_##type::_pool; \
std::vector<bool> Pool_##type::_free;

#endif

#pragma endregion pool_macro

DEFINE_POOL(BigInt);
DEFINE_POOL(Real);

#pragma endregion pool


#pragma region type_name

const char* type_names[Value::MAX_TYPE] = {
    "Int",
    "BigInt",
    "Real",
    "Vec2",
    "Vec3",
    "Vec4",
    "Mat2",
    "Mat3",
    "Mat4"
};

const char* Value::get_type_name(Type type) {
    #ifndef NDEBUG
    if (type >= MAX_TYPE) {
        Logger::log_err("Value::get_type_name called with invalid type %d!", type);
        throw std::out_of_range("Value::get_type_name called with invalid type, check log output for more information.");
    }
    #endif
    return type_names[type];
}

const char* Value::get_type_name() const {
    #ifndef NDEBUG
    if (type >= MAX_TYPE) {
        Logger::log_err("Value::get_type_name called with invalid type %d!", type);
        throw std::out_of_range("Value::get_type_name called with invalid type, check log output for more information.");
    }
    #endif
    return type_names[type];
}

#pragma endregion type_names


#pragma region converts

#ifdef NDEBUG
#define ASSERT_TYPE(t)
#else
#define ASSERT_TYPE(required_type) \
if (type != required_type) {\
    Logger::log_err("Value of type %s cannot be accessed as type %s!", get_type_name(), get_type_name(required_type));\
    throw std::logic_error("Value type mismatch, check log output for more information.");\
}
#endif

#define POOL_CONVERT(pool_type, enum_type) \
Value::operator pool_type() const { \
    ASSERT_TYPE(enum_type); \
    return Pool_##pool_type::get(data); \
}

Value::operator Int() const {
    ASSERT_TYPE(TYPE_INT);
    return std::bit_cast<Int>(data);
}

POOL_CONVERT(BigInt, TYPE_BIGINT);
POOL_CONVERT(Real, TYPE_REAL);

#undef ASSERT_TYPE
#undef POOL_CONVERT

#pragma endregion converts


#pragma region constructors

#define POOL_CONSTRUCT(pool_type, enum_type) Value::Value(pool_type value) : type(enum_type), data(Pool_##pool_type::allocate(value)) {}

Value::Value(Int value) : type(TYPE_INT), data(std::bit_cast<uint64_t>(value)) {}

POOL_CONSTRUCT(BigInt, TYPE_BIGINT);
POOL_CONSTRUCT(Real, TYPE_REAL);

#undef POOL_CONSTRUCT

#define POOL_FREE(pool_type, enum_type) \
case enum_type: { \
    Pool_##pool_type::free(data); \
    break; \
}

Value::~Value() {
    switch (type) {
        case TYPE_INT: { break; }

        POOL_FREE(BigInt, TYPE_BIGINT)
        POOL_FREE(Real, TYPE_REAL)

        default: {
            Logger::log_err("Value of type %s not handled during free!", get_type_name());
        }
    }
}

#undef POOL_FREE

Value::Value(Value&& value) {
    type = value.type;
    data = value.data;

    value.type = TYPE_INT;
    value.data = 0;
}

Value& Value::operator=(Value&& value) {
    type = value.type;
    data = value.data;

    value.type = TYPE_INT;
    value.data = 0;

    return *this;
}


Value Value::find_int(Real value, std::optional<const std::string*> source) {
    // No floating point, check for int64_t
    if (value <= (Real)std::numeric_limits<Int>::max() && value >= (Real)std::numeric_limits<Int>::min()) {
        return Value(static_cast<Int>(value));
    }
    if (source) {
        const std::string* str = source.value();
        if (str->starts_with("n")) {
            std::string new_str(*str);
            new_str.data()[0] = '-';
            return Value(new_str);
        }
        return Value(BigInt(*source.value()));
    }

    // BigInt, have to convert through string first
    const char* display_format = "%.0f";
    int size = snprintf(nullptr, 0, display_format, value) + 1;
    char* buf = (char*)malloc(sizeof(char) * size);
    memset(&buf[0], 0, size);
    snprintf(&buf[0], size, display_format, value);
    std::string str(&buf[0]);
    free(buf);

    return Value(BigInt(str));
}

#pragma endregion constructors

#pragma region parse

std::optional<Value> Value::parse(const std::string& str) {
    std::stringstream ss;
    std::string_view sv(str.c_str(), str.length());
    bool negate = false;

    // Check for negate
    if (str.starts_with('n')) {
        negate = true;
        sv = std::string_view(str.c_str() + 1, str.length() - 1);
    }

    // Try to parse as number first

    // Check for numeric prefixes
    if (sv.starts_with("0x")) {
        Int i_value;
        auto [ptr, ec] = std::from_chars(sv.data() + 2, sv.data() + sv.size(), i_value, 16);
        if (ec == std::errc()) {
            if (negate) { i_value *= -1; }
            return Value(i_value);
        }
    }
    else if (sv.starts_with("0o")) {
        Int i_value;
        auto [ptr, ec] = std::from_chars(sv.data() + 2, sv.data() + sv.size(), i_value, 8);
        if (ec == std::errc()) {
            if (negate) { i_value *= -1; }
            return Value(i_value);
        }
    }
    else {
        ss << sv;
    }

    Real d;
    ss >> d;

    if (ss && ss.eof()) {
        if (negate) { d *= -1; }
        return parse_numeric(str, d);
    }

    return std::nullopt;
}

Value Value::parse_numeric(const std::string& str, Real value) {
    // Check for floating point
    if (std::find(str.begin(), str.end(), '.') != str.end()) {
        // Contains a decimal separator, treat as float
        return Value(value);
    }
    
    return find_int(value, &str);
}

#pragma endregion parse


#pragma region to_string

std::string Value::to_string() {
    switch (type) {
        case TYPE_INT: {
            return std::to_string(operator Int());
        }
        case TYPE_BIGINT: {
            return (operator BigInt()).get_str();
        }
        case TYPE_REAL: {
            const char* display_format = "%g";
            const Real value = operator Real();
            int size = snprintf(nullptr, 0, display_format, value) + 1;
            if (size <= 512) {
                static char buf[512];
                memset(&buf[0], 0, 512);
                snprintf(&buf[0], 512, display_format, value);
                return std::string(&buf[0]);
            }
            // I tried snprintf'ing directly into a string and it caused The Problems I don't feel like solving right now.
            char* buf = (char*)malloc(sizeof(char) * size);
            memset(&buf[0], 0, size);
            snprintf(&buf[0], size, display_format, value);
            std::string str(&buf[0]);
            free(buf);
            return str;
        }
        default: {
            return "Value to_string not implemented for type!";
        }
    }
}

#pragma endregion to_string


#pragma region copies

Value Value::make_copy() const {
    switch (type) {
        case TYPE_INT: {
            return Value(operator Int());
        }
        case TYPE_BIGINT: {
            return Value(operator BigInt());
        }
        case TYPE_REAL: {
            return Value(operator Real());
        }
        default: {
            Logger::log_err("Value make_copy not implemented for type!");
            return Value((Int)0);
        }
    }
}

#pragma endregion copies
}