#include "core/error.h"

#include <iostream>


std::ostream& operator<<(std::ostream& os, const Err& error)
{
    os << "Error: " << error.get_type_str() << " (" << error.get_message() << ")";
    return os;
}


const char* Err::error_strings[] = {
    "Initialization Failure",
    "A specified parameter is invalid",
};


char ResultTypeMismatchException::expected_ok[] = "Result Type Unexpected (Was Ok, Attempted to unwrap Err)";
char ResultTypeMismatchException::expected_err[] = "Result Type Unexpected (Was Err, Attempted to unwrap Ok)";
