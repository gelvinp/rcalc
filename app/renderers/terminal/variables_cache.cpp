#include "variables_cache.h"
#include "scroll_component.h"
#include "app/application.h"

#include "ftxui/dom/table.hpp"

namespace RCalc::TerminalVariablesCache {

    ftxui::Component build_variables_cache(VariablesCacheData& data, Renderer::SubmitTextCallback cb_submit_text, bool& variables_close_requested, TerminalBackend& backend) {
    VariableMap& variables = cb_submit_text.p_app->get_variable_map();

    ftxui::Component v_lines = ftxui::Container::Vertical({});

    if (variables.get_values().empty()) {
        v_lines->Add(ftxui::Renderer([]{
            return ftxui::vbox({
                ftxui::text("No variables have been stored."),
            });
        }));
        
        return v_lines;
    }

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::vbox({
            ftxui::text("Variables"),
            ftxui::separatorEmpty(),
        });
    }));

    ftxui::Component layout = ftxui::Container::Vertical({});

    data.action_butttons.clear();
    data.menu_open.clear();
    data.menu_buttons.clear();
    data.modals.clear();
    data.menu_open.resize(variables.get_values().size(), false);

    size_t index = 0;
    for (const auto& [name, value] : variables.get_values()) {
        ftxui::Component menu = ftxui::Button(
            "Menu",
            [index, &data] {
                for (size_t menu_idx = 0; menu_idx < data.menu_open.size(); ++menu_idx) {
                    data.menu_open.mut_at(menu_idx) = menu_idx == index;
                }
            },
            ftxui::ButtonOption::Ascii()
        );

        data.menu_buttons.push_back({
            ftxui::Button(
                "Insert Name",
                [name, cb_submit_text, &variables_close_requested]() mutable {
                    std::string name_str = "\"" + name + "\"";
                    cb_submit_text(name_str);
                    variables_close_requested = true;
                },
                ftxui::ButtonOption::Ascii()
            ),
            ftxui::Button(
                "Insert Value",
                [name, cb_submit_text, &variables_close_requested]() mutable {
                    std::string name_str = "\"" + name + "\"";
                    cb_submit_text(name_str);
                    cb_submit_text("\\load");
                    variables_close_requested = true;
                },
                ftxui::ButtonOption::Ascii()
            ),

#ifdef MODULE_CLIP_ENABLED

            ftxui::Button(
                "Copy Name",
                [name, &variables_close_requested, &backend]() mutable {
                    std::string name_str = "\"" + name + "\"";
                    backend.copy_to_clipboard(name_str);
                    variables_close_requested = true;
                },
                ftxui::ButtonOption::Ascii()
            ),
            ftxui::Button(
                "Copy Value",
                [value, &variables_close_requested, &backend]() mutable {
                    std::string value_str = value.to_string();
                    backend.copy_to_clipboard(value_str);
                    variables_close_requested = true;
                },
                ftxui::ButtonOption::Ascii()
            ),

#endif

            ftxui::Button(
                "Remove",
                [name, &variables, &variables_close_requested, &data, index]() mutable {
                    variables.remove(name);
                    data.menu_open.mut_at(index) = false;
                },
                ftxui::ButtonOption::Ascii()
            ),
        });
        data.modals.push_back(ftxui::Container::Vertical(data.menu_buttons[index]));

        data.action_butttons.push_back(menu | ftxui::Modal(data.modals[index], &data.menu_open[index]));

        ++index;
    }
 
    layout->DetachAllChildren();
    for (auto comp : data.action_butttons) {
        layout->Add(std::move(comp));
    }

    ftxui::Component table = ftxui::Renderer(layout, [&] {
        std::vector<std::vector<ftxui::Element>> table_data;
        table_data.push_back({
            ftxui::text("Name") | ftxui::flex,
            ftxui::text("Value") | ftxui::flex,
            ftxui::text("Actions") | ftxui::flex
        });

        size_t index = 0;
        for (const auto& [name, value] : variables.get_values()) {
            table_data.push_back({
                ftxui::text(name) | ftxui::flex,
                ftxui::text(value.to_string()) | ftxui::flex,
                data.action_butttons[index]->Render() | ftxui::flex
            });
            index++;
        }
        
        ftxui::Table table { table_data };

        table.SelectAll().Border();
        table.SelectAll().SeparatorVertical();
        table.SelectRow(0).Decorate(ftxui::bold);
        table.SelectRow(0).Border(ftxui::DOUBLE);

        return table.Render();
    });

    v_lines->Add(table);
    
    return ScrollComponent::make(v_lines, false);
}

}