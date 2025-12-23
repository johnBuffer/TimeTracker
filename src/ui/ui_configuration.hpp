#pragma once
#include "SFML/Graphics/Color.hpp"

namespace ui
{

float constexpr outline_thickness = 0.0f;
float constexpr background_radius = 20.0f;
float constexpr background_shadow = 40.0f;
Vec2f constexpr outline_vec{outline_thickness, outline_thickness};

sf::Color constexpr background_color = {150, 150, 150};
sf::Color constexpr outline_color = {255, 255, 255};

size_t constexpr subtitle_size{20};
sf::Color constexpr subtitle_color = {50, 50, 50, 150};

size_t constexpr info_box_title_size = 32;
size_t constexpr info_box_value_size = 48;
size_t constexpr info_box_small_size = 24;

float constexpr margin = 40.0f;
float constexpr element_spacing = 10.0f;

}
