#include "value.h"

#include "core/comparison.h"
#include "core/logger.h"
#include "core/format.h"

#include <algorithm>
#include <bit>
#include <bitset>
#include <charconv>
#include <cstring>
#include <limits>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <typeinfo>

namespace RCalc {

#pragma region pool

#pragma region pool_macro

#ifdef TESTS_ENABLED
#define POOL_VISIBILITY public
#else
#define POOL_VISIBILITY private
#endif

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
POOL_VISIBILITY: \
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
POOL_VISIBILITY: \
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
DEFINE_POOL(Identifier);

#undef POOL_VISIBILITY

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
    "Unit",
    "Identifier",
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
POOL_CONVERT(Identifier, TYPE_IDENTIFIER);

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
POOL_CONSTRUCT(Identifier, TYPE_IDENTIFIER);

#undef POOL_CONSTRUCT

#define POOL_FREE(pool_type, enum_type) \
case enum_type: { \
    Pool_##pool_type::unref(data); \
    break; \
}

Value::~Value() {
    unref();
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

#pragma endregion constructors

#pragma region parse

std::optional<Value> Value::parse(std::string_view str) {
    // Check for vec or mat
    if (str.starts_with('[')) { return parse_vec(str); }
    if (str.starts_with('{')) { return parse_mat(str); }
    if (str.starts_with('_')) { return parse_unit(str); }
    if (str.starts_with('"') && str.ends_with('"')) { return parse_identifier(str); }

    return parse_scalar(str);
}


std::optional<Value> Value::parse_scalar(std::string_view sv) {
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
        str[0] = '-';
        sv = std::string_view(str);
    }

    // Check for numeric prefixes
    if (sv.starts_with("0x")) {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data() + 2, end, i_value, 16);
        if (ec == std::errc() && ptr == end) {
            return Value(i_value, REPR_HEX);
        }

        std::optional<std::string> bin_str = str_hex_to_bin(sv.data());
        if (bin_str) {
            return Value(BigInt(*str_bin_to_dec(*bin_str)), REPR_HEX);
        }
    }
    else if (sv.starts_with("0o")) {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data() + 2, end, i_value, 8);
        if (ec == std::errc() && ptr == end) {
            return Value(i_value, REPR_OCT);
        }

        std::optional<std::string> bin_str = str_oct_to_bin(sv.data());
        if (bin_str) {
            return Value(BigInt(*str_bin_to_dec(*bin_str)), REPR_OCT);
        }
    }
    else if (sv.starts_with("0b")) {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data() + 2, end, i_value, 2);
        if (ec == std::errc() && ptr == end) {
            return Value(i_value, REPR_BINARY);
        }

        std::optional<std::string> dec_str = str_bin_to_dec(sv.data());
        if (dec_str) {
            return Value(BigInt(dec_str.value()), REPR_BINARY);
        }
    }
    else {
        Int i_value;
        const char* end = sv.data() + sv.size();
        auto [ptr, ec] = std::from_chars(sv.data(), end, i_value, 10);
        if (ec == std::errc() && ptr == end) {
            return Value(i_value);
        }
        
        std::optional<BigInt> res = bigint::try_create(std::string(sv));
        if (res) {
            return Value(res.value());
        }
    }

    Real d_value;

#if defined(ENABLE_PLATFORM_MACOS) || defined(ENABLE_PLATFORM_IOS)
    // Floating point char_conv requires MacOS 13.3 or later.
    std::stringstream ss;
    ss << sv;
    ss >> d_value;
    if (ss && ss.eof()) {
        return Value(d_value, REPR_DECIMAL);
    }
#else
    const char* end = sv.data() + sv.size();
    auto [ptr, ec] = std::from_chars(sv.data(), end, d_value);
    if (ec == std::errc() && ptr == end) {
        return Value(d_value, REPR_DECIMAL);
    }
#endif

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
        std::optional<Value> component = parse_scalar(str);

        if (component) {
            if (components.size() >= 4) { return std::nullopt; }

            Real r_value = component->get_real();

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
            default:
                UNREACHABLE();
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
            UNREACHABLE();
    }
}

std::optional<Value> Value::parse_unit(std::string_view str) {
    std::optional<Unit const*> unit = UnitsMap::get_units_map().find_unit(str);
    if (unit) { return Value(*unit.value()); }
    return std::nullopt;
}

std::optional<Value> Value::parse_identifier(std::string_view str) {
    if (str.length() <= 2) { return std::nullopt; }
    Identifier identifier { str.substr(1, str.length() - 2) };
    return Value(identifier);
}

#pragma endregion parse


#pragma region str_convert

// It's damn slow but it's also correct. I might improve this later.
std::optional<std::string> Value::str_oct_to_bin(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return (char)std::tolower(ch); });
    if (!str.starts_with("0o")) { return std::nullopt; }
    if (str.find_first_not_of("01234567", 2) != str.npos) { return std::nullopt; }

    str = str.substr(2);

    std::string dec_str = "0b";

    for (char ch : str) {
        switch (ch) {
            case '0':
                dec_str.append("000");
                break;
            case '1':
                dec_str.append("001");
                break;
            case '2':
                dec_str.append("010");
                break;
            case '3':
                dec_str.append("011");
                break;
            case '4':
                dec_str.append("100");
                break;
            case '5':
                dec_str.append("101");
                break;
            case '6':
                dec_str.append("110");
                break;
            case '7':
                dec_str.append("111");
                break;
            default:
                throw std::logic_error("Cannot convert unexpected character from oct/hex to bin!");
        }
    }

    return dec_str;
}

std::optional<std::string> Value::str_hex_to_bin(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return (char)std::tolower(ch); });
    if (!str.starts_with("0x")) { return std::nullopt; }
    if (str.find_first_not_of("0123456789abcdef", 2) != str.npos) { return std::nullopt; }

    str = str.substr(2);

    std::string dec_str = "0b";

    for (char ch : str) {
        switch (ch) {
            case '0':
                dec_str.append("0000");
                break;
            case '1':
                dec_str.append("0001");
                break;
            case '2':
                dec_str.append("0010");
                break;
            case '3':
                dec_str.append("0011");
                break;
            case '4':
                dec_str.append("0100");
                break;
            case '5':
                dec_str.append("0101");
                break;
            case '6':
                dec_str.append("0110");
                break;
            case '7':
                dec_str.append("0111");
                break;
            case '8':
                dec_str.append("1000");
                break;
            case '9':
                dec_str.append("1001");
                break;
            case 'a':
                dec_str.append("1010");
                break;
            case 'b':
                dec_str.append("1011");
                break;
            case 'c':
                dec_str.append("1100");
                break;
            case 'd':
                dec_str.append("1101");
                break;
            case 'e':
                dec_str.append("1110");
                break;
            case 'f':
                dec_str.append("1111");
                break;
            default:
                throw std::logic_error("Cannot convert unexpected character from oct/hex to bin!");
        }
    }

    return dec_str;
}

std::optional<std::string> Value::str_bin_to_dec(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return (char)std::tolower(ch); });
    if (!str.starts_with("0b")) { return std::nullopt; }
    if (str.find_first_not_of("01", 2) != str.npos) { return std::nullopt; }

    str = str.substr(2);

    std::string dec_str;
    
    while (!str.empty() && str.find_first_not_of("0") != str.npos) {
        int remainder = 0;
        std::string working = "";

        for (char ch : str) {
            int bit = ch == '1' ? 1 : 0;
            remainder = 2 * remainder + bit;

            if (remainder < 10) {
                working.append("0");
            }
            else {
                working.append("1");
                remainder -= 10;
            }
        }

        constexpr static const char* decimals[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
        assert(remainder < 10);
        dec_str.append(decimals[remainder]);

        size_t start = working.find_first_not_of('0');
        if (start == working.npos) {
            str = working;
        }
        else {
            str = working.substr(start);
        }
    }

    std::reverse(dec_str.begin(), dec_str.end());
    return dec_str;
}

#pragma endregion str_convert


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

bool Value::is_negative() const {
    switch (type) {
        case TYPE_INT:
            return (operator Int()) < 0;
        case TYPE_BIGINT:
            return (operator BigInt()) < 0;
        case TYPE_REAL:
            return (operator Real()) < 0.0;

        default:
            return false;
    }
}

Real Value::get_real() const {
    switch (type) {
        case TYPE_INT:
            return Real(operator Int());
        case TYPE_BIGINT:
            return (operator BigInt()).get_real<Real>();
        case TYPE_REAL:
            return operator Real();

        default:
            throw std::logic_error("Cannot get non-scalar as a real!");
    }
}

#pragma endregion is_functions


#pragma region to_string

std::string Value::to_string(DisplayableTag tags, std::optional<int> precision) const {
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
                    return fmt("0o%llo", operator Int());
                case REPR_HEX:
                    return fmt("0x%llx", operator Int());
            }
            UNREACHABLE();
        }
        case TYPE_BIGINT: {
            return (operator BigInt()).get_str();
        }
        case TYPE_REAL: {
            char buf[RealFmtContainerSize];
            fmt_real(operator Real(), precision.value_or(_precision), { buf });
            return std::string { buf };
        }
        case TYPE_VEC2: {
            const Vec2 value = operator Vec2();

            if (tags == DisplayableTag::COL_VEC) {
                return fmt_col_vec2(value, precision.value_or(_precision));
            }

            switch (repr) {
                case REPR_HEX:
                    return fmt("[0x%x, 0x%x]", Int(value.x), Int(value.y));
                default:
                    char buf[2][RealFmtContainerSize];
                    fmt_real(value.x, precision.value_or(_precision), { buf[0] });
                    fmt_real(value.y, precision.value_or(_precision), { buf[1] });
                    return fmt("[%s, %s]", buf[0], buf[1]);
            }
        }
        case TYPE_VEC3: {
            const Vec3 value = operator Vec3();

            if (tags == DisplayableTag::COL_VEC) {
                return fmt_col_vec3(value, precision.value_or(_precision));
            }

            switch (repr) {
                case REPR_HEX:
                    return fmt("[0x%x, 0x%x, 0x%x]", Int(value.x), Int(value.y), Int(value.z));
                default:
                    char buf[3][RealFmtContainerSize];
                    fmt_real(value.x, precision.value_or(_precision), { buf[0] });
                    fmt_real(value.y, precision.value_or(_precision), { buf[1] });
                    fmt_real(value.z, precision.value_or(_precision), { buf[2] });
                    return fmt("[%s, %s, %s]", buf[0], buf[1], buf[2]);
            }
            UNREACHABLE();
        }
        case TYPE_VEC4: {
            const Vec4 value = operator Vec4();

            if (tags == DisplayableTag::COL_VEC) {
                return fmt_col_vec4(value, precision.value_or(_precision));
            }

            switch (repr) {
                case REPR_HEX:
                    return fmt("[0x%x, 0x%x, 0x%x, 0x%x]", Int(value.x), Int(value.y), Int(value.z), Int(value.w));
                default:
                    char buf[4][RealFmtContainerSize];
                    fmt_real(value.x, precision.value_or(_precision), { buf[0] });
                    fmt_real(value.y, precision.value_or(_precision), { buf[1] });
                    fmt_real(value.z, precision.value_or(_precision), { buf[2] });
                    fmt_real(value.w, precision.value_or(_precision), { buf[3] });
                    return fmt("[%s, %s, %s, %s]", buf[0], buf[1], buf[2], buf[3]);
            }
            UNREACHABLE();
        }
        case TYPE_MAT2: {
            const Mat2 value = operator Mat2();
            return fmt_mat2(value, precision.value_or(_precision), tags == DisplayableTag::ONE_LINE);
        }
        case TYPE_MAT3: {
            const Mat3 value = operator Mat3();
            return fmt_mat3(value, precision.value_or(_precision), tags == DisplayableTag::ONE_LINE);
        }
        case TYPE_MAT4: {
            const Mat4 value = operator Mat4();
            return fmt_mat4(value, precision.value_or(_precision), tags == DisplayableTag::ONE_LINE);
        }
        case TYPE_UNIT: {
            const Unit value = operator Unit();
            return fmt("%s", value.p_name);
        }
        case TYPE_IDENTIFIER: {
            const Identifier value = operator Identifier();
            return fmt("\"%s\"", value.c_str());
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
        POOL_REF(Identifier, TYPE_IDENTIFIER)

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
        POOL_UNREF(Identifier, TYPE_IDENTIFIER)

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
    return TypeComparison::compare(operator pool_type(), other.operator pool_type()); \
}

bool Value::operator==(const Value& other) const {
    if (type != other.type) { return false; }

    switch (type) {
        case TYPE_INT:
            return data == other.data;
        case TYPE_BIGINT:
            return operator BigInt() == other.operator BigInt();
        
        POOL_COMPARE(Real, TYPE_REAL)
        POOL_COMPARE(Vec2, TYPE_VEC2)
        POOL_COMPARE(Vec3, TYPE_VEC3)
        POOL_COMPARE(Vec4, TYPE_VEC4)
        POOL_COMPARE(Mat2, TYPE_MAT2)
        POOL_COMPARE(Mat3, TYPE_MAT3)
        POOL_COMPARE(Mat4, TYPE_MAT4)
        
        case TYPE_UNIT:
            return operator RCalc::Unit().p_impl == other.operator RCalc::Unit().p_impl;
        
        case TYPE_IDENTIFIER:
            return operator RCalc::Identifier() == other.operator RCalc::Identifier();

        default:
            UNREACHABLE();
    }
}

#undef POOL_COMPARE


#pragma endregion equality

#pragma region precision

int Value::_precision = 8;
std::string Value::_precision_fmt = "%.8g";

void Value::set_precision(int precision) {
    _precision = std::clamp(precision, 1, std::numeric_limits<Real>::max_digits10);
    _precision_fmt = fmt("%%.%dg", precision);
}

#pragma endregion precision
}