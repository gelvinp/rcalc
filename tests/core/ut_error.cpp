#include "snitch/snitch.hpp"
#include "core/checked_math.h"

#include <cstring>
#include <sstream>

using namespace RCalc;


TEST_CASE("Formats to ostream correctly", "[core][allocates]") {
    std::stringstream ss;
    ss << Err(ERR_INIT_FAILURE, "Test Failure!");

    REQUIRE(ss.str() == "Error: Initialization Failure (Test Failure!)");
}

TEST_CASE("Result type mismatch strings", "[core]") {
    SECTION("Ok unwrapped as Err") {
        try {
            Result<Value> res = Ok(Value());
            res.unwrap_err();
        }
        catch (const ResultTypeMismatchException& e) {
            REQUIRE(strcmp(e.what(), "Result Type Unexpected (Was Ok, Attempted to unwrap Err)") == 0);
        }
    }

    SECTION("Err unwrapped as Ok") {
        try {
            Result<Value> res = Err(ERR_INIT_FAILURE);
            res.unwrap();
        }
        catch (const ResultTypeMismatchException& e) {
            REQUIRE(strcmp(e.what(), "Result Type Unexpected (Was Err, Attempted to unwrap Ok)") == 0);
        }
    }

    SECTION("Err expected") {
        try {
            Result<Value> res = Err(ERR_INIT_FAILURE);
            res.expect("This is going to fail!");
        }
        catch (const ResultTypeMismatchException& e) {
            REQUIRE(strcmp(e.what(), "Result Type Unexpected (Was Err, Attempted to unwrap Ok)") == 0);
        }
    }

    SECTION("Err move unwrapped as Ok") {
        try {
            Result<Value> res = Err(ERR_INIT_FAILURE);
            res.unwrap_move(std::move(res));
        }
        catch (const ResultTypeMismatchException& e) {
            REQUIRE(strcmp(e.what(), "Result Type Unexpected (Was Err, Attempted to unwrap Ok)") == 0);
        }
    }

    SECTION("Err unwrapped with default") {
        Result<Value> res = Err(ERR_INIT_FAILURE);
        REQUIRE(res.unwrap_or(Value((Int)123)) == Value((Int)123));
    }
}

TEST_CASE("Can get Ok", "[core]") {
    Result<Value> res = Ok(Value((Int)123));

    REQUIRE(res.expect() == Value((Int)123));
    REQUIRE(res.expect("This will succeed!") == Value((Int)123));
    REQUIRE(res.unwrap() == Value((Int)123));
    REQUIRE(res.unwrap_or(Value((Int)456)) == Value((Int)123));
    REQUIRE(res.unwrap_move(std::move(res)) == Value((Int)123));
}