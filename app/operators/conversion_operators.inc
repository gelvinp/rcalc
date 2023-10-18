// @RCalcOperator
// Description: Converts arg0 between units arg1 and arg2 within the same family.
// Tags: [real_cast]
RCALC_OP_3(Convert, Real, Unit, Unit, arg0, arg1, arg2) {
    // Make sure units are in the same family
    if (arg1.p_family != arg2.p_family) {
        return Err(ERR_INVALID_PARAM, fmt("Units %s and %s are not in the same family!", arg1.p_name, arg2.p_name));
    }
    
    // Make sure unit family uses Real
    if (arg1.p_family->base_type != TYPE_REAL) {
        return Err(ERR_INVALID_PARAM, fmt("Unit family %s operates on type %s, not Real!", arg1.p_family->p_name, Value::get_type_name(arg1.p_family->base_type)));
    }

    Result<Value> base = arg1.to_base(Value(arg0));
    if (!base) { return base.unwrap_err(); }

    return arg2.from_base(base.unwrap_move(std::move(base)));
}

RCALC_OP_3(Convert, Vec2, Unit, Unit, arg0, arg1, arg2) {
    // Make sure units are in the same family
    if (arg1.p_family != arg2.p_family) {
        return Err(ERR_INVALID_PARAM, fmt("Units %s and %s are not in the same family!", arg1.p_name, arg2.p_name));
    }
    
    // Make sure unit family uses Vec2
    if (arg1.p_family->base_type != TYPE_VEC2) {
        return Err(ERR_INVALID_PARAM, fmt("Unit family %s operates on type %s, not Vec2!", arg1.p_family->p_name, Value::get_type_name(arg1.p_family->base_type)));
    }

    Result<Value> base = arg1.to_base(Value(arg0));
    if (!base) { return base.unwrap_err(); }

    return arg2.from_base(base.unwrap_move(std::move(base)));
}

RCALC_OP_3(Convert, Vec3, Unit, Unit, arg0, arg1, arg2) {
    // Make sure units are in the same family
    if (arg1.p_family != arg2.p_family) {
        return Err(ERR_INVALID_PARAM, fmt("Units %s and %s are not in the same family!", arg1.p_name, arg2.p_name));
    }
    
    // Make sure unit family uses Vec3
    if (arg1.p_family->base_type != TYPE_VEC3) {
        return Err(ERR_INVALID_PARAM, fmt("Unit family %s operates on type %s, not Vec3!", arg1.p_family->p_name, Value::get_type_name(arg1.p_family->base_type)));
    }

    Result<Value> base = arg1.to_base(Value(arg0));
    if (!base) { return base.unwrap_err(); }

    return arg2.from_base(base.unwrap_move(std::move(base)));
}

RCALC_OP_3(Convert, Vec4, Unit, Unit, arg0, arg1, arg2) {
    // Make sure units are in the same family
    if (arg1.p_family != arg2.p_family) {
        return Err(ERR_INVALID_PARAM, fmt("Units %s and %s are not in the same family!", arg1.p_name, arg2.p_name));
    }
    
    // Make sure unit family uses Vec4
    if (arg1.p_family->base_type != TYPE_VEC4) {
        return Err(ERR_INVALID_PARAM, fmt("Unit family %s operates on type %s, not Vec4!", arg1.p_family->p_name, Value::get_type_name(arg1.p_family->base_type)));
    }

    Result<Value> base = arg1.to_base(Value(arg0));
    if (!base) { return base.unwrap_err(); }

    return arg2.from_base(base.unwrap_move(std::move(base)));
}

RCALC_FMT_3(Convert, arg0, arg1, arg2) {
    return arg0.get_input_formatted() + " " + arg1.input + " to " + arg2.input;
}
