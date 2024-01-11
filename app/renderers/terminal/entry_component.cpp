#include "entry_component.h"

#include "terminal_renderer.h"


ftxui::Element StackEntryComponent::RenderEntry(std::optional<ftxui::Color> background) {
    auto flexconf = ftxui::FlexboxConfig()
        .Set(ftxui::FlexboxConfig::AlignItems::Center)
        .Set(ftxui::FlexboxConfig::AlignContent::FlexEnd);
    
    auto input_flow = ftxui::flexbox(input, flexconf);

    ftxui::Element output_element;

    if (std::find(output.begin(), output.end(), '\n') == output.end()) {
        output_element = ftxui::vbox({ ftxui::filler(), ftxui::text(output) });
    }
    else {
        // FTXUI ignores newlines in strings (even in paragraph blocks)
        output_element = ftxui::vbox(RCalc::TerminalRenderer::split_lines(output));
    }

    if (background) {
        auto input_bg = ftxui::hbox({
            input_flow | ftxui::bgcolor(background.value()),
            ftxui::filler()
        });

        return ftxui::hbox({
            input_bg,
            ftxui::filler(),
            ftxui::separatorEmpty(),
            output_element | ftxui::bgcolor(background.value())
        });
    }

    return ftxui::hbox({
        input_flow | ftxui::xflex_grow,
        ftxui::separatorEmpty(),
        output_element
    });
}


ftxui::Component StackEntryComponent::make(ftxui::Elements&& input, std::string&& output, RCalc::Value result) {
    return ftxui::Make<StackEntryComponent>(
        std::forward<ftxui::Elements&&>(input),
        std::forward<std::string&&>(output),
        std::forward<RCalc::Value>(result)
    );
}