#include "scroll_component.h"


// This file is inspired by https://github.com/ArthurSonzogni/git-tui/blob/master/src/scroller.cpp
// Available under the MIT license at https://github.com/ArthurSonzogni/git-tui/blob/master/LICENSE


ftxui::Element ScrollComponent::Render() {
    ftxui::Element element = ComponentBase::Render();
    element->ComputeRequirement();
    size = element->requirement().min_y;

    int ypos = 0;
    if (box.y_max > 0) {
        int screen_size = box.y_max - box.y_min;
        int screen_middle = screen_size / 2;

        scroll_offset = std::min(scroll_offset, size - 1);

        if (reverse) {
            int rev_offset = (size - screen_size) - scroll_offset;
            ypos = screen_middle + rev_offset;
        }
        else {
            ypos = screen_middle + scroll_offset;
        }
    }

    return element
        | ftxui::focusPosition(0, ypos)
        | ftxui::vscroll_indicator
        | ftxui::yframe
        | ftxui::reflect(box);
}


bool ScrollComponent::OnEvent(ftxui::Event event) {
    if (event == ftxui::Event::Home) {
        scroll_offset = reverse ? size : 0;
        return true;
    }
    if (event == ftxui::Event::End) {
        scroll_offset = reverse ? 0 : size;
        return true;
    }
    if (event == ftxui::Event::PageUp) {
        scroll_offset -= (box.y_max - box.y_min) * (reverse ? -1 : 1);
        return true;
    }
    if (event == ftxui::Event::PageDown) {
        scroll_offset += (box.y_max - box.y_min) * (reverse ? -1 : 1);
        return true;
    }

    if (
        (!control_scroll && event == ftxui::Event::ArrowUp) ||
        (event == ftxui::Event::ArrowUpCtrl) ||
        (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp)
     ) {
        if (reverse) {
            scroll_offset += 1;
        }
        else {
            if (scroll_offset > 0) { scroll_offset -= 1; }
        }
        return true;
    }
    if (
        (!control_scroll && event == ftxui::Event::ArrowDown) ||
        (event == ftxui::Event::ArrowDownCtrl) ||
        (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown)
     ) {
        if (reverse) {
            if (scroll_offset > 0) { scroll_offset -= 1; }
        }
        else {
            scroll_offset += 1;
        }
        return true;
    }

    if (children_.empty()) { return false; }
    return children_[0]->OnEvent(event);
}
