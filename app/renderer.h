#pragma once

#include "core/value.h"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace RCalc {

struct RenderItem {
    std::string_view input;
    std::string_view output;
};

class Renderer {
public:
    typedef std::function<void(const std::string&)> SubmitTextCallback;
    typedef std::function<void(const std::string&)> SubmitOperatorCallback;
    typedef std::function<void(const char*, const char*, const std::vector<const char*>&)> AppCommandCallback;
    typedef std::function<void(AppCommandCallback)> RequestAppCommandsCallback;
    typedef std::function<void(const char*, const char*, const std::vector<std::vector<Value::Type>>&)> OperatorCallback;
    typedef std::function<void(OperatorCallback)> RequestOperatorsCallback;

    static Renderer* create(const std::string& name, SubmitTextCallback cb_submit_text, SubmitOperatorCallback cb_submit_op, RequestAppCommandsCallback cb_request_app_cmds, RequestOperatorsCallback cb_request_ops);

    virtual void render(const std::vector<RenderItem>& items) = 0;

    virtual void display_info(const std::string& str) = 0;
    virtual void display_error(const std::string& str) = 0;

    virtual bool try_renderer_command(const std::string& str) = 0;
};

}