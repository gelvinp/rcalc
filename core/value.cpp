#include "value.h"

#include "core/logger.h"
#include "core/format.h"

#include <algorithm>
#include <bit>
#include <bitset>
#include <charconv>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <typeinfo>

namespace RCalc {

int Value::_precision = 12;

#pragma region pool

#pragma region pool_macro

#ifndef NDEBUG

#define DEFINE_POOL(type) \
class Pool_##type { \
public: \
    static uint64_t allocate(type value) { \
        if (_pool.size() != _refs.size()) { \
            Logger::log_err("[Allocate] Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _refs.size()); \
            throw std::logic_error("[Allocate] Pool size mismatch, check log output for more information."); \
        } \
\
        std::vector<uint8_t>::iterator it = std::find(_refs.begin(), _refs.end(), 0); \
        if (it != _refs.end()) { \
            /* Reuse slot */ \
            *it = 1; \
            uint64_t index = static_cast<uint64_t>(it - _refs.begin()); \
            _pool[index] = value; \
            return static_cast<uint64_t>(it - _refs.begin()); \
        } \
\
        if (_pool.size() > std::numeric_limits<uint64_t>::max()) { \
            Logger::log_err("Cannot allocate any more Values of type '" #type "'!"); \
            throw std::overflow_error("Value pool full, check log output for more information."); \
        } \
\
        /* New slot */ \
        _pool.push_back(value); \
        _refs.push_back(1); \
\
        return static_cast<uint64_t>(_pool.size() - 1); \
    } \
\
    static type get(uint64_t index) { \
        if (_pool.size() != _refs.size()) { \
            Logger::log_err("[Get] Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _refs.size()); \
            throw std::logic_error("[Get] Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("[Get] Pool access out of bounds for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("[Get] Pool free out of bounds, check log output for more information."); \
        } \
        if (_refs[index] == 0) { \
            Logger::log_err("[Get] Pool use after free for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("[Get] Pool use after free, check log output for more information."); \
        } \
\
        return _pool[index]; \
    } \
\
    static void ref(uint64_t index) { \
        if (_pool.size() != _refs.size()) { \
            Logger::log_err("[Ref] Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _refs.size()); \
            throw std::logic_error("[Ref] Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("[Ref] Pool access out of bounds for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("[Ref] Pool free out of bounds, check log output for more information."); \
        } \
        if (_refs[index] == 0) { \
            Logger::log_err("[Ref] Pool use after free for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("[Ref] Pool use after free, check log output for more information."); \
        } \
\
        _refs[index]++; \
    } \
\
    static void unref(uint64_t index) { \
        if (_pool.size() != _refs.size()) { \
            Logger::log_err("[Unref] Pool size mismatch for type '" #type "'!\n\tPool size: %d\tMarshall size: %d", _pool.size(), _refs.size()); \
            throw std::logic_error("[Unref] Pool size mismatch, check log output for more information."); \
        } \
        if (index >= _pool.size()) { \
            Logger::log_err("[Unref] Pool access out of bounds for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("[Unref] Pool free out of bounds, check log output for more information."); \
        } \
        if (_refs[index] == 0) { \
            Logger::log_err("[Unref] Pool use after free for type '" #type "'!\n\tPool size: %d\tAccessed index: %d", _pool.size(), index); \
            throw std::logic_error("[Unref] Pool use after free, check log output for more information."); \
        } \
\
        if (--_refs[index] > 0) { return; } \
\
        /* Attempt to shrink */ \
        std::vector<uint8_t>::const_iterator it = std::find_if(_refs.rbegin(), _refs.rend(), [](uint8_t e) { return e > 0; }).base(); \
        if (it == _refs.end()) { return; } \
        size_t new_size = it - _refs.begin(); \
\
        for (auto itd = it; itd < _refs.end(); ++itd) { \
            if (*itd) { \
                Logger::log_err("Pool shrink non-free value for type '" #type "'!\n\tPool size: %d\tFreed index: %d", _pool.size(), itd - _refs.begin()); \
            } \
        } \
\
        _pool.resize(new_size); \
        _refs.resize(new_size); \
    } \
\
private: \
    static std::vector<type> _pool; \
    static std::vector<uint8_t> _refs; \
}; \
\
std::vector<type> Pool_##type::_pool; \
std::vector<uint8_t> Pool_##type::_refs;

#else

#define DEFINE_POOL(type) \
class Pool_##type { \
public: \
    static uint64_t allocate(type value) { \
        std::vector<uint8_t>::iterator it = std::find(_refs.begin(), _refs.end(), 0); \
        if (it != _refs.end()) { \
            /* Reuse slot */ \
            *it = 1; \
            uint64_t index = static_cast<uint64_t>(it - _refs.begin()); \
            _pool[index] = value; \
            return static_cast<uint64_t>(it - _refs.begin()); \
        } \
\
        if (_pool.size() > std::numeric_limits<uint64_t>::max()) { \
            Logger::log_err("Cannot allocate any more Values of type '" #type "'!"); \
            throw std::overflow_error("Value pool full, check log output for more information."); \
        } \
\
        /* New slot */ \
        _pool.push_back(value); \
        _refs.push_back(1); \
\
        return static_cast<uint64_t>(_pool.size() - 1); \
    } \
\
    static type get(uint64_t index) { \
        return _pool[index]; \
    } \
\
    static void ref(uint64_t index) { \
        _refs[index]++; \
    } \
\
    static void unref(uint64_t index) { \
        if (--_refs[index] > 0) { return; } \
\
        /* Attempt to shrink */ \
        std::vector<uint8_t>::const_iterator it = std::find_if(_refs.rbegin(), _refs.rend(), [](uint8_t e) { return e > 0; }).base(); \
        if (it == _refs.end()) { return; } \
        size_t new_size = it - _refs.begin(); \
\
        _pool.resize(new_size); \
        _refs.resize(new_size); \
    } \
\
private: \
    static std::vector<type> _pool; \
    static std::vector<uint8_t> _refs; \
}; \
\
std::vector<type> Pool_##type::_pool; \
std::vector<uint8_t> Pool_##type::_refs;

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
DEFINE_POOL(Unit);

#pragma endregion pool


#pragma region type_name

const char* type_names[MAX_TYPE] = {
    "Int",
    "BigInt",
    "Real",
    "Vec2",
    "Vec3",
    "Vec4",
    "Mat2",
    "Mat3",
    "Mat4",
    "Unit"
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
POOL_CONVERT(Unit, TYPE_UNIT);

#undef ASSERT_TYPE
#undef POOL_CONVERT

#pragma endregion converts


#pragma region constructors

#define POOL_CONSTRUCT(pool_type, enum_type) Value::Value(pool_type value, Representation repr) : repr(repr), type(enum_type), data(Pool_##pool_type::allocate(value)) {}

Value::Value(Int value, Representation repr) : repr(repr), type(TYPE_INT), data(std::bit_cast<uint64_t>(value)) {}

Value::Value(Real value, Representation repr) : repr(repr), type(TYPE_REAL) {
    if (fabs(value) < 1e-15) {
        value = 0;
    }
    data = Pool_Real::allocate(value);
}

POOL_CONSTRUCT(BigInt, TYPE_BIGINT);
POOL_CONSTRUCT(Vec2, TYPE_VEC2);
POOL_CONSTRUCT(Vec3, TYPE_VEC3);
POOL_CONSTRUCT(Vec4, TYPE_VEC4);
POOL_CONSTRUCT(Mat2, TYPE_MAT2);
POOL_CONSTRUCT(Mat3, TYPE_MAT3);
POOL_CONSTRUCT(Mat4, TYPE_MAT4);
POOL_CONSTRUCT(Unit, TYPE_UNIT);

#undef POOL_CONSTRUCT

#define POOL_FREE(pool_type, enum_type) \
case enum_type: { \
    Pool_##pool_type::unref(data); \
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
        POOL_FREE(Unit, TYPE_UNIT)

        default: {
            Logger::log_err("Value of type %s not handled during free!", get_type_name());
        }
    }
}

#undef POOL_FREE

Value::Value(const Value& value) {
    type = value.type;
    data = value.data;
    repr = value.repr;
    ref();
}

Value::Value(Value&& value) : Value() {
    swap(value);
}

Value& Value::operator=(const Value& value) {
    Value other { value };
    swap(other);
    return *this;
}

Value& Value::operator=(Value&& value) {
    swap(value);
    return *this;
}


Value Value::find_int(Real value, std::optional<std::string_view> source, Representation repr) {
    // No floating point, check for int64_t
    if (value <= (Real)std::numeric_limits<Int>::max() && value >= (Real)std::numeric_limits<Int>::min()) {
        return Value(static_cast<Int>(value), repr);
    }
    if (source) {
        std::string str { source.value() };
        if (str.starts_with("n")) {
            str.data()[0] = '-';
            return Value(str, repr);
        }
        return Value(BigInt(str), repr);
    }

    // BigInt, have to convert through string first
    return Value(BigInt(fmt("%.0f", value)), repr);
}

#pragma endregion constructors

#pragma region parse

std::optional<Value> Value::parse(std::string_view str) {
    // Check for vec or mat
    if (str.starts_with('[')) { return parse_vec(str); }
    if (str.starts_with('{')) { return parse_mat(str); }
    if (str.starts_with('_')) { return parse_unit(str); }

    // Try to parse as number
    std::optional<Value> real = parse_real(str);

    if (real) {
        return parse_numeric(str, std::move(real).value(), real.value().repr);
    }

    return std::nullopt;
}

Value Value::parse_numeric(std::string_view str, Value&& value, Representation repr) {
    // Check for floating point
    if (std::find(str.begin(), str.end(), '.') != str.end() || std::find(str.begin(), str.end(), 'e') != str.end()) {
        // Contains a decimal separator, treat as float
        return value;
    }
    
    return find_int(value, str, repr);
}


std::optional<Value> Value::parse_real(std::string_view sv) {
    std::stringstream ss;
    bool negate = false;

    std::string str { sv };

    // Support 1en6
    auto exp_it = std::find(sv.begin(), sv.end(), 'e');
    if (exp_it != sv.end()) {
        size_t exp_idx = std::distance(sv.begin(), exp_it);
        str = std::string { sv };

        if ((exp_idx + 1) < str.size() && str[exp_idx + 1] == 'n') {
            str[exp_idx + 1] = '-';
        }

        sv = std::string_view(str);
    }

    // Check for negate
    if (sv.starts_with('n')) {
        negate = true;
        sv = std::string_view(sv.data() + 1, sv.length() - 1);
    }

    // Check for numeric prefixes
    if (sv.starts_with("0x")) {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data() + 2, end, i_value, 16);
        if (ec == std::errc() && ptr == end) {
            if (negate) { i_value *= -1; }
            return Value((Real)i_value, REPR_HEX);
        }
    }
    else if (sv.starts_with("0o")) {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data() + 2, end, i_value, 8);
        if (ec == std::errc() && ptr == end) {
            if (negate) { i_value *= -1; }
            return Value((Real)i_value, REPR_OCT);
        }
    }
    else if (sv.starts_with("0b")) {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data() + 2, end, i_value, 2);
        if (ec == std::errc() && ptr == end) {
            if (negate) { i_value *= -1; }
            return Value((Real)i_value, REPR_BINARY);
        }
    }
    else {
        ss << sv;
    }

    Real d_value;
    ss >> d_value;

    if (ss && ss.eof()) {
        if (negate) { d_value *= -1; }
            return Value(d_value, REPR_DECIMAL);
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
        sv = std::string_view { token };
        size_t begin = sv.find_first_not_of(" ");
        size_t end = sv.find_last_not_of(" ") + 1;

        // Negate
        bool negate = false;
        if (*(token + begin) == 'n') {
            negate = true;
            begin += 1;
        }

        std::string str { token + begin, end - begin };
        std::optional<Value> real = parse_real(str);

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
                if (value.value().type != TYPE_VEC2) { return std::nullopt; }
                break;
            }
            case 2: {
                if (value.value().type != TYPE_VEC3) { return std::nullopt; }
                break;
            }
            case 3: {
                if (value.value().type != TYPE_VEC4) { return std::nullopt; }
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

std::optional<Value> Value::parse_unit(std::string_view str) {
    std::optional<Unit const*> unit = UnitsMap::get_units_map().find_unit(str);
    if (unit) { return Value(*unit.value()); }
    return std::nullopt;
}

#pragma endregion parse


#pragma region is_functions

bool Value::is_vec() const {
    switch (type) {
        case TYPE_VEC2:
        case TYPE_VEC3:
        case TYPE_VEC4:
            return true;

        default:
            return false;
    }
}

bool Value::is_mat() const {
    switch (type) {
        case TYPE_MAT2:
        case TYPE_MAT3:
        case TYPE_MAT4:
            return true;

        default:
            return false;
    }
}

#pragma endregion is_functions


#pragma region to_string

std::string Value::to_string(DisplayableTag tags) const {
    switch (type) {
        case TYPE_INT: {
            switch (repr) {
                case REPR_NONE:
                // fallthrough
                case REPR_DECIMAL:
                    return std::to_string(operator Int());
                case REPR_BINARY: {
                    std::string str = std::bitset<64>(std::bit_cast<uint64_t>(operator Int())).to_string();
                    uint64_t offset = 64 - std::bit_width(std::bit_cast<uint64_t>(operator Int()));
                    return fmt("0b%s", str.c_str() + offset);
                }
                case REPR_OCT:
                    return fmt("0o%o", operator Int());
                case REPR_HEX:
                    return fmt("0x%x", operator Int());
            }
            UNREACHABLE();
        }
        case TYPE_BIGINT: {
            return (operator BigInt()).get_str();
        }
        case TYPE_REAL: {
            return fmt("%g", operator Real());
        }
        case TYPE_VEC2: {
            const Vec2 value = operator Vec2();

            if (tags == DisplayableTag::COL_VEC) {
                return fmt_col_vec2(value);
            }

            switch (repr) {
                case REPR_HEX:
                    return fmt("[0x%x, 0x%x]", Int(value.x), Int(value.y));
                default:
                    return fmt("[%g, %g]", value.x, value.y);
            }
        }
        case TYPE_VEC3: {
            const Vec3 value = operator Vec3();

            if (tags == DisplayableTag::COL_VEC) {
                return fmt_col_vec3(value);
            }

            switch (repr) {
                case REPR_HEX:
                    return fmt("[0x%x, 0x%x, 0x%x]", Int(value.x), Int(value.y), Int(value.z));
                default:
                    return fmt("[%g, %g, %g]", value.x, value.y, value.z);
            }
            UNREACHABLE();
        }
        case TYPE_VEC4: {
            const Vec4 value = operator Vec4();

            if (tags == DisplayableTag::COL_VEC) {
                return fmt_col_vec4(value);
            }

            switch (repr) {
                case REPR_HEX:
                    return fmt("[0x%x, 0x%x, 0x%x, 0x%x]", Int(value.x), Int(value.y), Int(value.z), Int(value.w));
                default:
                    return fmt("[%g, %g, %g, %g]", value.x, value.y, value.z, value.w);
            }
            UNREACHABLE();
        }
        case TYPE_MAT2: {
            const Mat2 value = operator Mat2();
            return fmt_mat2(value, tags == DisplayableTag::ONE_LINE);
        }
        case TYPE_MAT3: {
            const Mat3 value = operator Mat3();
            return fmt_mat3(value, tags == DisplayableTag::ONE_LINE);
        }
        case TYPE_MAT4: {
            const Mat4 value = operator Mat4();
            return fmt_mat4(value, tags == DisplayableTag::ONE_LINE);
        }
        case TYPE_UNIT: {
            const Unit value = operator Unit();
            return fmt("%s", value.p_name);
        }

        default: {
            Logger::log_err("Value of type %s not handled during to_string!", get_type_name());
            return fmt("Value of type %s not handled during to_string!", get_type_name());
        }
    }
}

#pragma endregion to_string


#pragma region refcounted

#define POOL_REF(pool_type, enum_type) \
case enum_type: { \
    Pool_##pool_type::ref(data); \
    return; \
}

void Value::ref() {
    switch (type) {
        case TYPE_INT:
            return;

        POOL_REF(BigInt, TYPE_BIGINT)
        POOL_REF(Real, TYPE_REAL)
        POOL_REF(Vec2, TYPE_VEC2)
        POOL_REF(Vec3, TYPE_VEC3)
        POOL_REF(Vec4, TYPE_VEC4)
        POOL_REF(Mat2, TYPE_MAT2)
        POOL_REF(Mat3, TYPE_MAT3)
        POOL_REF(Mat4, TYPE_MAT4)
        POOL_REF(Unit, TYPE_UNIT)

        default: {
            Logger::log_err("Value of type %s not handled during ref!", get_type_name());
        }
    }
}

#undef POOL_REF

#define POOL_UNREF(pool_type, enum_type) \
case enum_type: { \
    Pool_##pool_type::unref(data); \
    return; \
}

void Value::unref() {
    switch (type) {
        case TYPE_INT:
            return;

        POOL_UNREF(BigInt, TYPE_BIGINT)
        POOL_UNREF(Real, TYPE_REAL)
        POOL_UNREF(Vec2, TYPE_VEC2)
        POOL_UNREF(Vec3, TYPE_VEC3)
        POOL_UNREF(Vec4, TYPE_VEC4)
        POOL_UNREF(Mat2, TYPE_MAT2)
        POOL_UNREF(Mat3, TYPE_MAT3)
        POOL_UNREF(Mat4, TYPE_MAT4)
        POOL_UNREF(Unit, TYPE_UNIT)

        default: {
            Logger::log_err("Value of type %s not handled during unref!", get_type_name());
        }
    }
}

#undef POOL_UNREF

#pragma endregion refcounted

#pragma region equality

#define POOL_COMPARE(pool_type, enum_type) \
case enum_type: { \
    return operator pool_type() == other.operator pool_type(); \
}

bool Value::operator==(const Value& other) const {
    if (type != other.type) { return false; }

    switch (type) {
        case TYPE_INT:
            return data == other.data;
        
        POOL_COMPARE(BigInt, TYPE_BIGINT)
        POOL_COMPARE(Real, TYPE_REAL)
        POOL_COMPARE(Vec2, TYPE_VEC2)
        POOL_COMPARE(Vec3, TYPE_VEC3)
        POOL_COMPARE(Vec4, TYPE_VEC4)
        POOL_COMPARE(Mat2, TYPE_MAT2)
        POOL_COMPARE(Mat3, TYPE_MAT3)
        POOL_COMPARE(Mat4, TYPE_MAT4)
        
        case TYPE_UNIT:
            return operator RCalc::Unit().p_impl == other.operator RCalc::Unit().p_impl;

        default:
            UNREACHABLE();
    }
}

#undef POOL_COMPARE

#pragma endregion equality
}