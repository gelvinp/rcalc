// @RCalcOperator
// Description: Calculate the minimum of the top arg0 values on the stack.
// Category: Statistics
// Tags: [no_expr]
// Requires: <cmath>
// Requires: <sstream>
// Example: [1, 2, 3, 3]
RCALC_OP_1_S(Min, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute min, not enough elements on the stack.");
    }

    Type smallest_scalar = TYPE_INT;
    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
            break;
        case TYPE_BIGINT:
            smallest_scalar = std::max(smallest_scalar, TYPE_BIGINT);
            break;
        case TYPE_REAL:
            smallest_scalar = std::max(smallest_scalar, TYPE_REAL);
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute min, all elements must be scalar types.");
        }
    }

    // For input formatting
    std::stringstream ss;
    bool first = true;

    switch (smallest_scalar) {
        case TYPE_INT:
        {
            std::vector<Int> values;
            for (StackItem& item : stack.pop_items(count)) {
                values.push_back((Int)item.result);
                if (first) { first = false; } else { ss << ", "; }
                ss << item.result.to_string();
            }
            std::vector<Int>::iterator min = std::min_element(values.begin(), values.end());
            stack.set_message<std::string>(ss.str());
            return Ok(Value(*min));
        }
        case TYPE_BIGINT:
        {
            std::vector<BigInt> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case TYPE_INT:
                    values.push_back(BigInt((long long int)(item.result.operator Int())));
                    break;
                case TYPE_BIGINT:
                    values.push_back(item.result.operator BigInt());
                    break;
                default:
                    UNREACHABLE();
                }
                if (first) { first = false; } else { ss << ", "; }
                ss << item.result.to_string();
            }
            std::vector<BigInt>::iterator min = std::min_element(values.begin(), values.end());
            stack.set_message<std::string>(ss.str());
            return Ok(Value(*min));
        }
        case TYPE_REAL:
        {
            std::vector<Real> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case TYPE_INT:
                    values.push_back((Real)(item.result.operator Int()));
                    break;
                case TYPE_BIGINT:
                    values.push_back((item.result.operator BigInt()).get_real<Real>());
                    break;
                case TYPE_REAL:
                    values.push_back((Real)item.result);
                    break;
                default:
                    UNREACHABLE();
                }
                if (first) { first = false; } else { ss << ", "; }
                ss << item.result.to_string();
            }
            std::vector<Real>::iterator min = std::min_element(values.begin(), values.end());
            stack.set_message<std::string>(ss.str());
            return Ok(Value(*min));
        }
        default:
            UNREACHABLE();
    }
}

RCALC_FMT_1_S(Min, _arg) {
    UNUSED(_arg);
    std::string str = "min(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Description: Calculate the maximum of the top arg0 values on the stack.
// Category: Statistics
// Tags: [no_expr]
// Requires: <cmath>
// Requires: <sstream>
// Example: [1, 2, 3, 3]
RCALC_OP_1_S(Max, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute max, not enough elements on the stack.");
    }

    Type smallest_scalar = TYPE_INT;
    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
            break;
        case TYPE_BIGINT:
            smallest_scalar = std::max(smallest_scalar, TYPE_BIGINT);
            break;
        case TYPE_REAL:
            smallest_scalar = std::max(smallest_scalar, TYPE_REAL);
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute max, all elements must be scalar types.");
        }
    }

    // For input formatting
    std::stringstream ss;
    bool first = true;

    switch (smallest_scalar) {
        case TYPE_INT:
        {
            std::vector<Int> values;
            for (StackItem& item : stack.pop_items(count)) {
                values.push_back((Int)item.result);
                if (first) { first = false; } else { ss << ", "; }
                ss << item.result.to_string();
            }
            std::vector<Int>::iterator max = std::max_element(values.begin(), values.end());
            stack.set_message<std::string>(ss.str());
            return Ok(Value(*max));
        }
        case TYPE_BIGINT:
        {
            std::vector<BigInt> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case TYPE_INT:
                    values.push_back(BigInt((long long int)(item.result.operator Int())));
                    break;
                case TYPE_BIGINT:
                    values.push_back(item.result.operator BigInt());
                    break;
                default:
                    UNREACHABLE();
                }
                if (first) { first = false; } else { ss << ", "; }
                ss << item.result.to_string();
            }
            std::vector<BigInt>::iterator max = std::max_element(values.begin(), values.end());
            stack.set_message<std::string>(ss.str());
            return Ok(Value(*max));
        }
        case TYPE_REAL:
        {
            std::vector<Real> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case TYPE_INT:
                    values.push_back((Real)(item.result.operator Int()));
                    break;
                case TYPE_BIGINT:
                    values.push_back((item.result.operator BigInt()).get_real<Real>());
                    break;
                case TYPE_REAL:
                    values.push_back((Real)item.result);
                    break;
                default:
                    UNREACHABLE();
                }
                if (first) { first = false; } else { ss << ", "; }
                ss << item.result.to_string();
            }
            std::vector<Real>::iterator max = std::max_element(values.begin(), values.end());
            stack.set_message<std::string>(ss.str());
            return Ok(Value(*max));
        }
        default:
            UNREACHABLE();
    }
}

RCALC_FMT_1_S(Max, _arg) {
    UNUSED(_arg);
    std::string str = "max(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Description: Calculate the average of the top arg0 values on the stack.
// Category: Statistics
// Tags: [no_expr]
// Requires: <numeric>
// Requires: <sstream>
// Example: [1, 2, 3, 3]
RCALC_OP_1_S(Avg, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute avg, not enough elements on the stack.");
    }

    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
        case TYPE_BIGINT:
        case TYPE_REAL:
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute avg, all elements must be scalar types.");
        }
    }

    std::stringstream ss;
    std::vector<Real> values = get_reals(stack.pop_items(count), ss);
    
    Real avg = std::accumulate(values.begin(), values.end(), 0.0) / count;
    stack.set_message<std::string>(ss.str());
    return Ok(Value(avg));
}

RCALC_FMT_1_S(Avg, _arg) {
    UNUSED(_arg);
    std::string str = "avg(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Description: Calculate the standard deviation of the top arg0 values on the stack.
// Category: Statistics
// Tags: [no_expr]
// Requires: <numeric>
// Requires: <sstream>
// Requires: <cmath>
// Requires: "app/operators/stats.h"
// Example: [1, 2, 3, 3]
RCALC_OP_1_S(StdDev, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute stddev, not enough elements on the stack.");
    }

    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
        case TYPE_BIGINT:
        case TYPE_REAL:
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute stddev, all elements must be scalar types.");
        }
    }

    std::stringstream ss;
    std::vector<Real> values = get_reals(stack.pop_items(count), ss);
    
    Real avg = std::accumulate(values.begin(), values.end(), 0.0) / count;
    stack.set_message<std::string>(ss.str());

    std::vector<Real> sqr_dev;
    sqr_dev.reserve(count);

    for (Real real : values) {
        Real dev = real - avg;
        sqr_dev.push_back(dev * dev);
    }

    Real variance = std::accumulate(sqr_dev.begin(), sqr_dev.end(), 0.0) / count;
    return Ok(Value(std::sqrt(variance)));
}

RCALC_FMT_1_S(StdDev, _arg) {
    UNUSED(_arg);
    std::string str = "stddev(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Description: Calculate the [average, standard deviation] of the top arg0 values on the stack.
// Category: Statistics
// Tags: [no_expr]
// Requires: <numeric>
// Requires: <sstream>
// Requires: <cmath>
// Requires: "app/operators/stats.h"
// Example: [1, 2, 3, 3]
RCALC_OP_1_S(NormDist, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute stddev, not enough elements on the stack.");
    }

    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
        case TYPE_BIGINT:
        case TYPE_REAL:
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute stddev, all elements must be scalar types.");
        }
    }

    std::stringstream ss;
    std::vector<Real> values = get_reals(stack.pop_items(count), ss);
    
    Real avg = std::accumulate(values.begin(), values.end(), 0.0) / count;
    stack.set_message<std::string>(ss.str());

    std::vector<Real> sqr_dev;
    sqr_dev.reserve(count);

    for (Real real : values) {
        Real dev = real - avg;
        sqr_dev.push_back(dev * dev);
    }

    Real variance = std::accumulate(sqr_dev.begin(), sqr_dev.end(), 0.0) / count;
    return Ok(Value(Vec2(avg, std::sqrt(variance))));
}

RCALC_FMT_1_S(NormDist, _arg) {
    UNUSED(_arg);
    std::string str = "normdist(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Description: Calculate the sum of the top arg0 values on the stack.
// Category: Statistics
// Tags: [no_expr]
// Requires: <numeric>
// Requires: <sstream>
// Example: [1, 2, 3, 3]
RCALC_OP_1_S(Sum, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute sum, not enough elements on the stack.");
    }

    bool is_real = false;

    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
        case TYPE_BIGINT:
            break;
        case TYPE_REAL:
            is_real = true;
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute sum, all elements must be scalar types.");
        }
    }

    // For input formatting
    std::stringstream ss;
    bool first = true;

    Real sum = 0.0;
    for (StackItem& item : stack.pop_items(count)) {
        switch (item.result.get_type()) {
        case TYPE_INT:
            sum += (Real)(item.result.operator Int());
            break;
        case TYPE_BIGINT:
            sum += (item.result.operator BigInt()).get_real<Real>();
            break;
        case TYPE_REAL:
            sum += (Real)item.result;
            break;
        default:
            UNREACHABLE();
        }
        if (first) { first = false; } else { ss << ", "; }
        ss << item.result.to_string();
    }

    stack.set_message<std::string>(ss.str());

    if (is_real) {
        return Ok(Value(sum));
    }
    else {
        return Ok(Value::find_int(sum));
    }
}

RCALC_FMT_1_S(Sum, _arg) {
    UNUSED(_arg);
    std::string str = "sum(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Description: Given the discount rate arg0, calculates the net present value of the top arg1 values on the stack.
// Category: Statistics
// Tags: [no_expr, real_cast]
// Requires: <algorithm>
// Requires: <sstream>
// Requires: <cmath>
// Requires: "app/operators/stats.h"
// Example: [-20000, 12000, 14000, 16000, 0.1, 4]
RCALC_OP_2_S(NPV, Real, Int, discount_rate, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute net present value, not enough net cash flows on the stack.");
    }

    for (Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case TYPE_INT:
        case TYPE_BIGINT:
        case TYPE_REAL:
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute net present value, all net cash flows must be scalar types.");
        }
    }

    std::stringstream ss;
    std::vector<Real> values = get_reals(stack.pop_items(count), ss);
    stack.set_message<std::string>(ss.str());

    size_t index = 0;
    std::transform(values.begin(), values.end(), values.begin(),
        [&index, discount_rate](Real net_cash_flow) {
            return net_cash_flow / std::pow(1 + discount_rate, index++);
        }
    );

    Real npv = std::accumulate(values.begin(), values.end(), 0.0);
    return Ok(Value(npv));
}

RCALC_FMT_2_S(NPV, discount_rate, _arg) {
    UNUSED(_arg);
    std::string str = "npv[" + discount_rate.get_input_formatted() + "](" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}