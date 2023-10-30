#include "entry_component.h"


ftxui::Element StackEntryComponent::RenderEntry(std::optional<ftxui::Color> background) {
    auto flexconf = ftxui::FlexboxConfig()
        .Set(ftxui::FlexboxConfig::AlignItems::Center)
        .Set(ftxui::FlexboxConfig::AlignContent::FlexEnd);
    
    auto input_flow = ftxui::flexbox(input, flexconf);

    if (background) {
        auto input_bg = ftxui::hbox({
            input_flow | ftxui::bgcolor(background.value()),
            ftxui::filler()
        });

        return ftxui::hbox({
            input_bg,
            ftxui::filler(),
            ftxui::separatorEmpty(),
            output | ftxui::bgcolor(background.value())
        });
    }

    return ftxui::hbox({
        input_flow | ftxui::xflex_grow,
        ftxui::separatorEmpty(),
        output
    });
}


ftxui::Component StackEntryComponent::make(ftxui::Elements&& input, ftxui::Element&& output) {
    return ftxui::Make<StackEntryComponent>(
        std::forward<ftxui::Elements&&>(input),
        std::forward<ftxui::Element&&>(output)
    );
}