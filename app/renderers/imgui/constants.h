#pragma once

#include "imgui.h"

namespace RCalc {
namespace ImGuiRendererConstants {

const float STACK_HORIZ_PADDING = 16.0f;
const float STACK_HORIZ_BIAS = 8.0f;
const float STACK_OUTER_PADDING = 4.0f;
const float STACK_BOTTOM_PADDING = 8.0f;
const float STACK_BETWEEN_PADDING = 6.0f;

namespace ColorPalettes {

constexpr ImVec4 COLORS_DARK[] = {
    { 1.0f, 0.239f, 0.239f, 1.0f },     // Red
    { 1.0f, 0.619f, 0.329f, 1.0f },     // Orange
    { 1.0f, 0.909f, 0.388f, 1.0f },     // Yellow
    { 0.439f, 1.0f, 0.388f, 1.0f },     // Green
    { 0.412f, 0.655f, 1.0f, 1.0f },     // Blue
    { 0.698f, 0.451f, 1.0f, 1.0f },     // Purple
    { 1.0f, 1.0f, 1.0f, 1.0f },         // White
    { 1.0f, 0.561f, 0.847f, 1.0f },     // Pink
    { 0.309f, 0.898f, 1.0f, 1.0f },     // Light Blue
    { 0.361f, 0.196f, 0.039f, 1.0f },   // Brown
    { 0.0f, 0.0f, 0.0f, 1.0f },         // Black
    { 0.8f, 0.8f, 0.8f, 1.0f },         // Gray
};

constexpr ImVec4 COLORS_LIGHT[] = {
    { 0.933f, 0.216f, 0.173f, 1.0f },     // Red
    { 0.969f, 0.550f, 0.016f, 1.0f },     // Orange
    { 0.979f, 0.930f, 0.142f, 1.0f },     // Yellow
    { 0.192f, 0.620f, 0.075f, 1.0f },     // Green
    { 0.129f, 0.424f, 0.933f, 1.0f },     // Blue
    { 0.584f, 0.192f, 1.0f, 1.0f },     // Purple
    { 1.0f, 1.0f, 1.0f, 1.0f },         // White
    { 1.0f, 0.360f, 0.777f, 1.0f },     // Pink
    { 0.094f, 0.866f, 1.0f, 1.0f },     // Light Blue
    { 0.427f, 0.275f, 0.133f, 1.0f },   // Brown
    { 0.0f, 0.0f, 0.0f, 1.0f },         // Black
    { 0.084f, 0.101f, 0.122f, 1.0f },         // Gray
};

}

enum ColorNames : int {
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_WHITE,
    COLOR_PINK,
    COLOR_LIGHT_BLUE,
    COLOR_BROWN,
    COLOR_BLACK,
    COLOR_GRAY
};

}
}