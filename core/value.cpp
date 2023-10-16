#include "value.h"

#include "core/logger.h"
#include "core/format.h"

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
DEFINE_POOL(Vec2);
DEFINE_POOL(Vec3);
DEFINE_POOL(Vec4);
DEFINE_POOL(Mat2);
DEFINE_POOL(Mat3);
DEFINE_POOL(Mat4);

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
POOL_CONVERT(Vec2, TYPE_VEC2);
POOL_CONVERT(Vec3, TYPE_VEC3);
POOL_CONVERT(Vec4, TYPE_VEC4);
POOL_CONVERT(Mat2, TYPE_MAT2);
POOL_CONVERT(Mat3, TYPE_MAT3);
POOL_CONVERT(Mat4, TYPE_MAT4);

#undef ASSERT_TYPE
#undef POOL_CONVERT

#pragma endregion converts


#pragma region constructors

#define POOL_CONSTRUCT(pool_type, enum_type) Value::Value(pool_type value) : type(enum_type), data(Pool_##pool_type::allocate(value)) {}

Value::Value(Int value) : type(TYPE_INT), data(std::bit_cast<uint64_t>(value)) {}

POOL_CONSTRUCT(BigInt, TYPE_BIGINT);
POOL_CONSTRUCT(Real, TYPE_REAL);
POOL_CONSTRUCT(Vec2, TYPE_VEC2);
POOL_CONSTRUCT(Vec3, TYPE_VEC3);
POOL_CONSTRUCT(Vec4, TYPE_VEC4);
POOL_CONSTRUCT(Mat2, TYPE_MAT2);
POOL_CONSTRUCT(Mat3, TYPE_MAT3);
POOL_CONSTRUCT(Mat4, TYPE_MAT4);

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
        POOL_FREE(Vec2, TYPE_VEC2)
        POOL_FREE(Vec3, TYPE_VEC3)
        POOL_FREE(Vec4, TYPE_VEC4)
        POOL_FREE(Mat2, TYPE_MAT2)
        POOL_FREE(Mat3, TYPE_MAT3)
        POOL_FREE(Mat4, TYPE_MAT4)

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
    // Check for vec or mat
    if (str.starts_with('[')) { return parse_vec(str); }
    if (str.starts_with('{')) { return parse_mat(str); }

    // Try to parse as number
    std::optional<Real> real = parse_real(str);

    if (real) {
        return parse_numeric(str, real.value());
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


std::optional<Real> Value::parse_real(std::string_view sv) {
    std::stringstream ss;
    bool negate = false;

    // Check for negate
    if (sv.starts_with('n')) {
        negate = true;
        sv = std::string_view(sv.data() + 1, sv.length() - 1);
    }

    // Try to parse as number

    // Check for numeric prefixes
    if (sv.starts_with("0x")) {
        Int i_value;
        auto [ptr, ec] = std::from_chars(sv.data() + 2, sv.data() + sv.size(), i_value, 16);
        if (ec == std::errc()) {
            if (negate) { i_value *= -1; }
            return i_value;
        }
    }
    else if (sv.starts_with("0o")) {
        Int i_value;
        auto [ptr, ec] = std::from_chars(sv.data() + 2, sv.data() + sv.size(), i_value, 8);
        if (ec == std::errc()) {
            if (negate) { i_value *= -1; }
            return i_value;
        }
    }
    else {
        ss << sv;
    }

    Real d_value;
    ss >> d_value;

    if (ss && ss.eof()) {
        if (negate) { d_value *= -1; }
        return d_value;
    }

    return std::nullopt;
}


std::optional<Value> Value::parse_vec(std::string_view sv) {
    if (!sv.starts_with('[') || !sv.ends_with(']')) { return std::nullopt; }

    std::string source { sv.data() + 1, sv.length() - 2 };
    const char* delims = ",";
    char* ctx = nullptr;
    std::vector<Real> components;

    char* token = strtok_p(source.data(), delims, &ctx);
    while (token) {
        // Trim whitespace
        std::string_view sv(token);
        size_t begin = sv.find_first_not_of(" ");
        size_t end = sv.find_last_not_of(" ") + 1;

        // Negate
        bool negate = false;
        if (*(token + begin) == 'n') {
            negate = true;
            begin += 1;
        }

        sv = std::string_view(token + begin, end - begin);
        std::optional<Real> real = parse_real(sv);

        if (real) {
            if (components.size() >= 4) { return std::nullopt; }

            Real r_value = real.value();

            if (negate) { r_value *= -1; }

            components.push_back(r_value);
            token = strtok_p(nullptr, delims, &ctx);
        }
        else { return std::nullopt; }
    }

    switch (components.size()) {
        case 2:
            return Value(Vec2(components[0], components[1]));
        case 3:
            return Value(Vec3(components[0], components[1], components[2]));
        case 4:
            return Value(Vec4(components[0], components[1], components[2], components[3]));
        default:
            return std::nullopt;
    }
}


// Parsing as row-major i.e.
// {[a11, a12], [a21, a22]} -> |a11 a12|
//                             |a21 a22|
std::optional<Value> Value::parse_mat(std::string_view sv) {
    if (!sv.starts_with('{') || !sv.ends_with('}')) { return std::nullopt; }

    // Find sub-vectors
    size_t split_loc[3] = {};
    size_t split_count = 0;
    bool in_vec = false;
    for (size_t idx = 0; idx < sv.size(); ++idx) {
        switch (sv[idx]) {
            case '[': {
                if (in_vec) { return std::nullopt; } // {[[1]]} <- Invalid, no nested []'s allowed
                in_vec = true;
                break;
            }
            case ']': {
                if (!in_vec) { return std::nullopt; } // {], 1} <- Invalid, No mismatched []'s allowed
                in_vec = false;
                break;
            }
            case ',': {
                if (in_vec) { break; } // {[1, 2], []} <- Ignore commas inside subvecs
                if (split_count >= 3) { return std::nullopt; } // {[], [], [], [], []} <- Invalid, matrices more than 4x4 not allowed
                split_loc[split_count++] = idx;
            }
        }
    }
    if (split_count == 0) { return std::nullopt; } // {} OR {[]} <- Invalid, must be at least 2x2

    // Parse sub-vectors
    Value sub_vecs[4] = {};
    for (size_t idx = 0; idx <= split_count; ++idx) {
        size_t split_from = (idx == 0)           ? /* Skip beginning { */ 1               : /* Go from after last comma */ (split_loc[idx - 1] + 1);
        size_t split_to   = (idx == split_count) ? /* Skip ending } */    (sv.size() - 1) : /* Go until next comma */      (split_loc[idx]);
        std::string_view subvec_sv { sv.begin() + split_from, sv.begin() + split_to };

        size_t ws_begin = subvec_sv.find_first_not_of(" ");
        size_t ws_end = subvec_sv.find_last_not_of(" ") + 1;
        subvec_sv = { subvec_sv.begin() + ws_begin, subvec_sv.begin() + ws_end };
        
        std::optional<Value> value = parse_vec(subvec_sv);
        if (!value) { return std::nullopt; }
        switch (split_count) {
            case 1: {
                if (value.value().type != Value::TYPE_VEC2) { return std::nullopt; }
                break;
            }
            case 2: {
                if (value.value().type != Value::TYPE_VEC3) { return std::nullopt; }
                break;
            }
            case 3: {
                if (value.value().type != Value::TYPE_VEC4) { return std::nullopt; }
                break;
            }
        }
        sub_vecs[idx] = std::move(value.value());
    }

    switch (split_count) {
        case 1: {
            Mat2 mat {sub_vecs[0], sub_vecs[1]};
            return Value(glm::transpose(mat));
        }
        case 2: {
            Mat3 mat {sub_vecs[0], sub_vecs[1], sub_vecs[2]};
            return Value(glm::transpose(mat));
        }
        case 3: {
            Mat4 mat {sub_vecs[0], sub_vecs[1], sub_vecs[2], sub_vecs[3]};
            return Value(glm::transpose(mat));
        }
        default:
            return std::nullopt;
    }
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
            return fmt("%g", operator Real());
        }
        case TYPE_VEC2: {
            const Vec2 value = operator Vec2();
            return fmt("[%g, %g]", value.x, value.y);
        }
        case TYPE_VEC3: {
            const Vec3 value = operator Vec3();
            return fmt("[%g, %g, %g]", value.x, value.y, value.z);
        }
        case TYPE_VEC4: {
            const Vec4 value = operator Vec4();
            return fmt("[%g, %g, %g, %g]", value.x, value.y, value.z, value.w);
        }
        case TYPE_MAT2: {
            const Mat2 value = operator Mat2();
            return fmt("{[%g, %g], [%g, %g]}", value[0].x, value[1].x, value[0].y, value[1].y);
        }
        case TYPE_MAT3: {
            const Mat3 value = operator Mat3();
            return fmt("{[%g, %g, %g], [%g, %g, %g], [%g, %g, %g]}", value[0].x, value[1].x, value[2].x, value[0].y, value[1].y, value[2].y, value[0].z, value[1].z, value[2].z);
        }
        case TYPE_MAT4: {
            const Mat4 value = operator Mat4();
            return fmt("{[%g, %g, %g, %g], [%g, %g, %g, %g], [%g, %g, %g, %g], [%g, %g, %g, %g]}", value[0].x, value[1].x, value[2].x, value[3].x, value[0].y, value[1].y, value[2].y, value[3].y, value[0].z, value[1].z, value[2].z, value[3].z, value[0].w, value[1].w, value[2].w, value[3].w);
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
        case TYPE_VEC2: {
            return Value(operator Vec2());
        }
        case TYPE_VEC3: {
            return Value(operator Vec3());
        }
        case TYPE_VEC4: {
            return Value(operator Vec4());
        }
        case TYPE_MAT2: {
            return Value(operator Mat2());
        }
        case TYPE_MAT3: {
            return Value(operator Mat3());
        }
        case TYPE_MAT4: {
            return Value(operator Mat4());
        }
        default: {
            Logger::log_err("Value make_copy not implemented for type!");
            return Value((Int)0);
        }
    }
}

#pragma endregion copies
}