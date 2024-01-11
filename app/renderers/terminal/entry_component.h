#pragma once

#include "core/value.h"
#include "ftxui/component/component.hpp"

#include <optional>
#include <string>


class StackEntryComponent : public ftxui::ComponentBase {
public:
    explicit StackEntryComponent(ftxui::Elements&& input, std::string&& output, RCalc::Value result)
        : input(std::move(input)), output(std::move(output)), result(result) {}
    
    ftxui::Element Render() override  { return RenderEntry(); }
    ftxui::Element RenderEntry(std::optional<ftxui::Color> background = std::nullopt);

    static ftxui::Component make(ftxui::Elements&& input, std::string&& output, RCalc::Value result);

    ftxui::Elements input;
    std::string output;
    RCalc::Value result;
};