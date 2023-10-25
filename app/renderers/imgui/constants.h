#pragma once

#include "imgui.h"

namespace RCalc {

const float STACK_HORIZ_PADDING = 16.0;
const float STACK_HORIZ_BIAS = 8.0;
const float STACK_OUTER_PADDING = 4.0;
const float STACK_BOTTOM_PADDING = 8.0;

constexpr ImVec4 COLORS[] = {
    { 1.0, 0.239, 0.239, 1.0 },     // Red
    { 1.0, 0.619, 0.329, 1.0 },   // Orange
    { 1.0, 0.909, 0.388, 1.0 },     // Yellow
    { 0.439, 1.0, 0.388, 1.0 },     // Green
    { 0.412, 0.655, 1.0, 1.0 },     // Blue
    { 0.698, 0.451, 1.0, 1.0 },     // Purple
    { 1.0, 1.0, 1.0, 1.0 },         // White
    { 1.0, 0.561, 0.847, 1.0 },     // Pink
    { 0.309, 0.898, 1.0, 1.0 },     // Light Blue
    { 0.361, 0.196, 0.039, 1.0 },   // Brown
    { 0.0, 0.0, 0.0, 1.0 },         // Black
    { 0.8, 0.8, 0.8, 1.0 },         // Gray
};

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