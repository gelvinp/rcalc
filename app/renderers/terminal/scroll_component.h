#pragma once

#include "ftxui/component/component.hpp"


// This file is inspired by https://github.com/ArthurSonzogni/git-tui/blob/master/src/scroller.cpp
// Available under the MIT license at https://github.com/ArthurSonzogni/git-tui/blob/master/LICENSE


class ScrollComponent : public ftxui::ComponentBase {
public:
    explicit ScrollComponent(ftxui::Component child, bool control_scroll, bool reverse)
        : control_scroll(control_scroll), reverse(reverse) { Add(child); }
    
    ftxui::Element Render() override;
    bool OnEvent(ftxui::Event event) override;

    static ftxui::Component make(ftxui::Component child, bool control_scroll = true, bool reverse = false) {
        return ftxui::Make<ScrollComponent>(std::move(child), control_scroll, reverse);
    }

private:
    bool control_scroll;
    bool reverse;

    int scroll_offset = 0;
    int size;
    ftxui::Box box;
};
