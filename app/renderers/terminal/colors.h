#pragma once

#include "ftxui/screen/color.hpp"

#include <cstdint>

namespace RCalc {

constexpr ftxui::Color::Palette16 COLORS_16[] = {
    ftxui::Color::Palette16::Red,           // Red
    ftxui::Color::Palette16::RedLight,      // Orange
    ftxui::Color::Palette16::YellowLight,   // Yellow
    ftxui::Color::Palette16::GreenLight,    // Green
    ftxui::Color::Palette16::BlueLight,     // Blue
    ftxui::Color::Palette16::Magenta,       // Purple
    ftxui::Color::Palette16::White,         // White
    ftxui::Color::Palette16::MagentaLight,  // Pink
    ftxui::Color::Palette16::CyanLight,     // Light Blue
    ftxui::Color::Palette16::GrayLight,     // Brown
    ftxui::Color::Palette16::Black,         // Black
    ftxui::Color::Palette16::GrayDark,      // Gray
};

constexpr ftxui::Color::Palette256 COLORS_256[] = {
    ftxui::Color::Palette256::Red1,              // Red
    ftxui::Color::Palette256::DarkOrange,          // Orange
    ftxui::Color::Palette256::Yellow1,          // Yellow
    ftxui::Color::Palette256::SeaGreen2,     // Green
    ftxui::Color::Palette256::DodgerBlue1,     // Blue
    ftxui::Color::Palette256::MediumPurple2,    // Purple
    ftxui::Color::Palette256::Grey100,            // White
    ftxui::Color::Palette256::Orchid2,          // Pink
    ftxui::Color::Palette256::SkyBlue1,         // Light Blue
    ftxui::Color::Palette256::Orange4Bis,       // Brown
    ftxui::Color::Palette256::Grey0,            // Black
    ftxui::Color::Palette256::Grey74,           // Gray
};

constexpr uint8_t COLORS_TRUE[][3] = {
    { 255, 61, 61 },      // Red
    { 255, 158, 84 },     // Orange
    { 255, 232, 99 },     // Yellow
    { 112, 255, 99 },     // Green
    { 105, 167, 255 },    // Blue
    { 178, 115, 255 },    // Purple
    { 255, 255, 255 },    // White
    { 255, 143, 216 },    // Pink
    { 79, 229, 255 },     // Light Blue
    { 92, 50, 10 },       // Brown
    { 0, 0, 0 },          // Black
    { 204, 204, 204 },    // Gray
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

#define SUPPORTS_PALETTE(palette) ftxui::Terminal::ColorSupport() >= ftxui::Terminal::Color::palette

}