#include "value.h"

#include "core/logger.h"

#include <algorithm>
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
            Logger::log_err("Pool size mismatch for type '%s'!\n\tPool size: %d\tMarshall size: %d", typeid(type).name(), _pool.size(), _free.size()); \
            throw std::logic_error("Pool size mismatch, check log output for more information."); \
        } \
\
        std::vector<bool>::iterator it = std::find(_free.begin(), _free.end(), false); \
        if (it != _free.end()) { \
            /* Reuse slot */ \
            *it = true; \
            return static_cast<uint64_t>(it - _free.begin()); \
        } \
\
        if (_pool.size() > std::numeric_limits<uint64_t>::max()) { \
            Logger::log_err("Cannot allocate any more Values of type '%s'!", typeid(type).name()); \
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
            Logger::log_err("Pool size mismatch for type '%s'!\n\tPool size: %d\tMarshall size: %d", typeid(type).name(), _pool.size(), _free.size()); \
            throw std::logic_error("Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("Pool access out of bounds for type '%s'!\n\tPool size: %d\tAccessed index: %d", typeid(type).name(), _pool.size(), index); \
            throw std::logic_error("Pool free out of bounds, check log output for more information."); \
        } \
        if (_free[index] == false) { \
            Logger::log_err("Pool use after free for type '%s'!\n\tPool size: %d\tAccessed index: %d", typeid(type).name(), _pool.size(), index); \
            throw std::logic_error("Pool use after free, check log output for more information."); \
        } \
\
        return _pool[index]; \
    } \
\
    static void free(uint64_t index) { \
        if (_pool.size() != _free.size()) { \
            Logger::log_err("Pool size mismatch for type '%s'!\n\tPool size: %d\tMarshall size: %d", typeid(type).name(), _pool.size(), _free.size()); \
            throw std::logic_error("Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("Pool free out of bounds for type '%s'!\n\tPool size: %d\tFreed index: %d", typeid(type).name(), _pool.size(), index); \
            throw std::logic_error("Pool free out of bounds, check log output for more information."); \
        } \
        if (_free[index] == false) { \
            Logger::log_err("Pool double free for type '%s'!\n\tPool size: %d\tFreed index: %d", typeid(type).name(), _pool.size(), index); \
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
                Logger::log_err("Pool shrink non-free value for type '%s'!\n\tPool size: %d\tFreed index: %d", typeid(type).name(), _pool.size(), itd - _free.begin()); \
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
            return static_cast<uint64_t>(it - _free.begin()); \
        } \
\
        if (_pool.size() > std::numeric_limits<uint64_t>::max()) { \
            Logger::log_err("Cannot allocate any more Values of type '%s'!", typeid(type).name()); \
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
        if (_pool.size() != _free.size()) { \
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

DEFINE_POOL(double);

#pragma endregion pool


#pragma region type_name

const char* type_names[Value::MAX_TYPE] = {
    "Int",
    "Double",
    "BigInt",
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

Value::operator int64_t() const {
    ASSERT_TYPE(TYPE_INT);
    return std::bit_cast<int64_t>(data);
}

POOL_CONVERT(double, TYPE_DOUBLE);

#undef ASSERT_TYPE
#undef POOL_CONVERT

#pragma endregion converts


#pragma region constructors

#define POOL_CONSTRUCT(pool_type, enum_type) Value::Value(pool_type value) : type(enum_type), data(Pool_##pool_type::allocate(value)) {}

Value::Value(int64_t value) : type(TYPE_INT), data(std::bit_cast<uint64_t>(value)) {}

POOL_CONSTRUCT(double, TYPE_DOUBLE);

#undef POOL_CONSTRUCT

#define POOL_FREE(pool_type, enum_type) \
case enum_type: { \
    Pool_##pool_type::free(data); \
    break; \
}

Value::~Value() {
    switch (type) {
        case TYPE_INT: { break; }

        POOL_FREE(double, TYPE_DOUBLE)

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

#pragma endregion constructors

#pragma region parse

std::optional<Value> Value::parse(const std::string& str) {
    std::stringstream ss;
    ss.str(str);

    // Try to parse as number first

    // Check for numeric prefixes
    if (str.starts_with("0x")) {
        ss.flags(ss.flags() | std::ios_base::hex);
    }
    else if (str.starts_with("0o")) {
        ss.flags(ss.flags() | std::ios_base::oct);
    }

    double d;
    ss >> d;

    if (ss && ss.eof()) {
        return parse_numeric(str, d);
    }

    return std::nullopt;
}

Value Value::parse_numeric(const std::string& str, double value) {
    // Check for floating point
    if (std::find(str.begin(), str.end(), '.') != str.end()) {
        // Contains a decimal separator, treat as float
        return Value(value);
    }

    // No floating point, check for int64_t
    if (value <= std::numeric_limits<int64_t>::max() && value >= std::numeric_limits<int64_t>::min()) {
        return Value(static_cast<int64_t>(value));
    }

    // Too big, must use a BigInt
    Logger::log_err("TODO: BigInt");
    return Value((int64_t)0);
}

#pragma endregion parse
}