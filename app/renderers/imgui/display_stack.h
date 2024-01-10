#pragma once

#include "imgui.h"
#include "app/stack.h"

#include <string>
#include <optional>
#include <vector>


namespace RCalc {

struct ImGuiDisplayChunk {
    std::string str;
    ImVec2 size;
    ImVec2 position;

    void calculate_size(float max_width = -1.0f);

    ImGuiDisplayChunk(std::string str = "")
        : str(str), size({}), position({}) {}
};

struct ImGuiDisplayLine {
    std::vector<ImGuiDisplayChunk> chunks;
    ImVec2 size;

    void calculate_size(float max_width = -1.0f);
};

struct ImGuiDisplayEntry {
    ImGuiDisplayLine input;
    ImGuiDisplayChunk output;
    float height;
    bool valid = false;
    
    ImGuiDisplayEntry(const StackItem& item, std::optional<int> precision = std::nullopt);

    void calculate_size(float max_width, bool scrollbar_visible, bool bottom_align = true);
};

struct ImGuiDisplayStack {
    std::vector<ImGuiDisplayEntry> entries;

    void calculate_sizes(float max_width, bool scrollbar_visible, bool bottom_align = true);
    void invalidate_sizes();
};

}