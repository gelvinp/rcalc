#pragma once

#include "app/renderers/renderer.h"
#include "app/stack.h"

namespace RCalc {

class RendererTemplate : public Renderer {
public:
    RendererTemplate(SubmitTextCallback cb_submit_text);

    virtual Result<> init() override;
    virtual void render_loop() override;
    virtual void cleanup() override;

    virtual void display_info(std::string_view str) override;
    virtual void display_error(std::string_view str) override;

    virtual bool try_renderer_command(std::string_view str) override;

    virtual void add_stack_item(const StackItem& item) override;
    virtual void remove_stack_item() override;
    virtual void replace_stack_items(const CowVec<StackItem>& items) override;
};

}