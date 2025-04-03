#pragma once

#include "app/renderers/renderer.h"
#include "app/stack.h"

namespace RCalc {

class DummyRenderer : public Renderer {
public:
    DummyRenderer();

    virtual void early_init(const AppConfig& config, SubmitTextCallback submit_text) override;
    virtual Result<> init() override;
    virtual void render_loop() override;
    virtual void cleanup() override;

    virtual void display_info(std::string_view str) override;
    virtual void display_error(std::string_view str) override;

    virtual bool try_renderer_command(std::string_view str) override;

    virtual void add_stack_item(const StackItem& item) override;
    virtual void remove_stack_item() override;
    virtual void replace_stack_items(const CowVec<StackItem>& items) override;

    std::string last_message = "";
    bool last_message_was_error = false;
};

}