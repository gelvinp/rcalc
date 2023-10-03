// @RCalcOperator
// Name: Min
// Parameters: [Int]
// Description: Calculate the minimum of the top arg0 values on the stack.
// Tags: [no_expr, stack_ref, fmt_stack_ref]
// Requires: <cmath>
// Requires: <sstream>
RCALC_OP_1_S(Min, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute min, not enough elements on the stack.");
    }

    Value::Type smallest_scalar = Value::TYPE_INT;
    for (Value::Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case Value::TYPE_INT:
            break;
        case Value::TYPE_BIGINT:
            smallest_scalar = std::max(smallest_scalar, Value::TYPE_BIGINT);
            break;
        case Value::TYPE_REAL:
            smallest_scalar = std::max(smallest_scalar, Value::TYPE_REAL);
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute min, all elements must be scalar types.");
        }
    }

    // For input formatting
    std::stringstream ss;
    bool first = true;

    switch (smallest_scalar) {
        case Value::TYPE_INT:
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
        case Value::TYPE_BIGINT:
        {
            std::vector<BigInt> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case Value::TYPE_INT:
                    values.push_back(BigInt((long long int)(item.result.operator Int())));
                    break;
                case Value::TYPE_BIGINT:
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
        case Value::TYPE_REAL:
        {
            std::vector<Real> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case Value::TYPE_INT:
                    values.push_back((Real)(item.result.operator Int()));
                    break;
                case Value::TYPE_BIGINT:
                    values.push_back((item.result.operator BigInt()).get_real<Real>());
                    break;
                case Value::TYPE_REAL:
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
// Name: Max
// Parameters: [Int]
// Description: Calculate the maximum of the top arg0 values on the stack.
// Tags: [no_expr, stack_ref, fmt_stack_ref]
// Requires: <cmath>
// Requires: <sstream>
RCALC_OP_1_S(Max, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute max, not enough elements on the stack.");
    }

    Value::Type smallest_scalar = Value::TYPE_INT;
    for (Value::Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case Value::TYPE_INT:
            break;
        case Value::TYPE_BIGINT:
            smallest_scalar = std::max(smallest_scalar, Value::TYPE_BIGINT);
            break;
        case Value::TYPE_REAL:
            smallest_scalar = std::max(smallest_scalar, Value::TYPE_REAL);
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute max, all elements must be scalar types.");
        }
    }

    // For input formatting
    std::stringstream ss;
    bool first = true;

    switch (smallest_scalar) {
        case Value::TYPE_INT:
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
        case Value::TYPE_BIGINT:
        {
            std::vector<BigInt> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case Value::TYPE_INT:
                    values.push_back(BigInt((long long int)(item.result.operator Int())));
                    break;
                case Value::TYPE_BIGINT:
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
        case Value::TYPE_REAL:
        {
            std::vector<Real> values;
            for (StackItem& item : stack.pop_items(count)) {
                switch (item.result.get_type()) {
                case Value::TYPE_INT:
                    values.push_back((Real)(item.result.operator Int()));
                    break;
                case Value::TYPE_BIGINT:
                    values.push_back((item.result.operator BigInt()).get_real<Real>());
                    break;
                case Value::TYPE_REAL:
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
// Name: Avg
// Parameters: [Int]
// Description: Calculate the average of the top arg0 values on the stack.
// Tags: [no_expr, stack_ref, fmt_stack_ref]
// Requires: <numeric>
// Requires: <sstream>
RCALC_OP_1_S(Avg, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute avg, not enough elements on the stack.");
    }

    for (Value::Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case Value::TYPE_INT:
        case Value::TYPE_BIGINT:
        case Value::TYPE_REAL:
            break;
        default:
            return Err(ERR_INVALID_PARAM, "Cannot compute avg, all elements must be scalar types.");
        }
    }

    // For input formatting
    std::stringstream ss;
    bool first = true;

    std::vector<Real> values;
    for (StackItem& item : stack.pop_items(count)) {
        switch (item.result.get_type()) {
        case Value::TYPE_INT:
            values.push_back((Real)(item.result.operator Int()));
            break;
        case Value::TYPE_BIGINT:
            values.push_back((item.result.operator BigInt()).get_real<Real>());
            break;
        case Value::TYPE_REAL:
            values.push_back((Real)item.result);
            break;
        default:
            UNREACHABLE();
        }
        if (first) { first = false; } else { ss << ", "; }
        ss << item.result.to_string();
    }
    
    Real sum = std::accumulate(values.begin(), values.end(), 0.0);
    stack.set_message<std::string>(ss.str());
    return Ok(Value(sum / count));
}

RCALC_FMT_1_S(Avg, _arg) {
    UNUSED(_arg);
    std::string str = "avg(" + stack.get_message<std::string>() + ")";
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Name: Sum
// Parameters: [Int]
// Description: Calculate the sum of the top arg0 values on the stack.
// Tags: [no_expr, stack_ref, fmt_stack_ref]
// Requires: <numeric>
// Requires: <sstream>
RCALC_OP_1_S(Sum, Int, count) {
    if (stack.size() < (size_t)count) {
        return Err(ERR_INVALID_PARAM, "Cannot compute sum, not enough elements on the stack.");
    }

    bool is_real = false;

    for (Value::Type type : stack.peek_types_vec(count)) {
        switch (type) {
        case Value::TYPE_INT:
        case Value::TYPE_BIGINT:
            break;
        case Value::TYPE_REAL:
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
        case Value::TYPE_INT:
            sum += (Real)(item.result.operator Int());
            break;
        case Value::TYPE_BIGINT:
            sum += (item.result.operator BigInt()).get_real<Real>();
            break;
        case Value::TYPE_REAL:
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