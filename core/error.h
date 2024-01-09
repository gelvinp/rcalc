#pragma once

#include "core/logger.h"

#include <concepts>
#include <cstdio>
#include <exception>
#include <functional>
#include <string>
#include <variant>

namespace RCalc {


#pragma region Ok
namespace OkTypes
{
    template <typename T>
    class Ok
    {
    public:
        Ok(const T &ok) : data(ok) {}
        Ok(T &&ok) : data(std::move(ok)) {}

        const T &get() const & { return data; }
        T &&get() && { return std::move(data); }

    private:
        T data;
    };

    template <>
    class Ok<void>
    {
    };
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const OkTypes::Ok<T> &ok)
{
    os << ok.get();
    return os;
}

template <typename T, typename Decayed = typename std::decay<T>::type>
OkTypes::Ok<Decayed> Ok(T &&ok)
{
    return OkTypes::Ok<Decayed>(std::forward<T>(ok));
}

inline OkTypes::Ok<void> Ok()
{
    return OkTypes::Ok<void>();
}
#pragma endregion Ok


#pragma region Err
enum ErrType
{
    ERR_INIT_FAILURE,
    ERR_INVALID_PARAM,
    ERR_LOAD_FAILURE,
    ERR_SAVE_FAILURE
};

class Err
{
public:
    Err(ErrType type, std::string message = "") : type(type), message(message) {}

    ErrType get_type() const { return type; }
    const char *get_type_str() const { return error_strings[type]; }
    const std::string &get_message() const { return message; }

    friend std::ostream &operator<<(std::ostream &os, const Err &error);

private:
    ErrType type;
    std::string message;
    static const char *error_strings[];
};
#pragma endregion


#pragma region Result
class ResultTypeMismatchException : public std::exception
{
public:
    ResultTypeMismatchException(bool was_ok) : was_ok(was_ok) {}
    virtual const char *what() const noexcept override
    {
        if (was_ok)
        {
            return expected_ok;
        }
        return expected_err;
    }

private:
    bool was_ok;

    static char expected_ok[];
    static char expected_err[];
};

template <typename T = void>
class Result
{
public:
    Result(OkTypes::Ok<T> &&ok) : data(std::move(ok)) {}
    Result(Err &&err) : data(std::move(err)) {}

    operator bool() { return data.index() == 0; }

    template <typename U = T>
    typename std::enable_if<!std::is_same<U, void>::value, U>::type unwrap() const
    {
        if (data.index() == 0)
        {
            return std::get<OkTypes::Ok<U>>(data).get();
        }

        Logger::log_err("CRITICAL ERROR: Attempted to call unwrap() on an Err Result!");
        throw ResultTypeMismatchException(false);
    }

    template <typename U = T>
    static typename std::enable_if<!std::is_same<U, void>::value, U&&>::type unwrap_move(Result<T>&& res)
    {
        if (res.data.index() == 0)
        {
            return std::get<OkTypes::Ok<U>>(std::move(res.data)).get();
        }

        Logger::log_err("CRITICAL ERROR: Attempted to call unwrap() on an Err Result!");
        throw ResultTypeMismatchException(false);
    }

    template <typename U = T>
    typename std::enable_if<std::is_same<U, void>::value, U&&>::type unwrap() const
    {
    }

    template <typename U = T>
    static typename std::enable_if<std::is_same<U, void>::value, U>::type unwrap_move()
    {
    }

    template <typename U = T, typename V = OkTypes::Ok<U>>
    typename std::enable_if<!std::is_same<U, void>::value, V>::type unwrap_or(const U &default_value) const
    {
        if (data.index() == 0)
        {
            return std::get<V>(data);
        }

        return default_value;
    }

    T expect(const char *message)
    {
        if (data.index() == 0)
        {
            return std::get<OkTypes::Ok<T>>(data).get();
        }

        Logger::log_err("CRITICAL ERROR: %s", message);
        throw ResultTypeMismatchException(false);
    }

    T expect()
    {
        return unwrap();
    }

    Err unwrap_err()
    {
        if (data.index() == 1)
        {
            return std::get<Err>(data);
        }

        Logger::log_err("CRITICAL ERROR: Attempted to call unwrap_err() on an Ok Result!");
        throw ResultTypeMismatchException(true);
    }

    template<typename U, typename F>
    typename std::enable_if<std::is_same<T, void>::value, Result<U>>::type and_then(F next)
    {
        if (data.index() == 0) {
            return next();
        }

        return unwrap_err();
    }

    template<typename U, typename F>
    typename std::enable_if<!std::is_same<T, void>::value, Result<U>>::type and_then(F next) {
        if (data.index() == 0) {
            return next(std::get<OkTypes::Ok<T>>(data).get());
        }

        return unwrap_err();
    }

private:
    std::variant<OkTypes::Ok<T>, Err> data;
};
#pragma endregion Result


}


// Utility for short-circuiting any errors
#define UNWRAP_R(expr)                 \
    {                                  \
        auto __res = expr;             \
        if (!__res)                    \
        {                              \
            return __res.unwrap_err(); \
        }                              \
    }
