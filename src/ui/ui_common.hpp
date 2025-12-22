#pragma once
#include "peztool/utils/render/card/card_outlined.hpp"

#include "./ui_configuration.hpp"
#include "standard/widget.hpp"

namespace ui
{
[[nodiscard]]
inline pez::CardOutlined createBackground(Vec2f const size)
{
    pez::CardOutlined card{size, background_radius, outline_thickness, background_color};
    card.setOutlineShadowSize(0.0f);
    card.setBackgroundShadowSize(0.0f * background_shadow);
    card.background.blur_background = false;
    card.background.shadow_offset = {0.0f, 7.0f};
    card.setOutlineColor(outline_color);
    return card;
}

[[nodiscard]]
inline sf::Text createTitle(sf::Font const& font, std::string const& str)
{
    sf::Text text{font, str, title_size};
    text.setFillColor(title_color);
    return text;
}

[[nodiscard]]
inline sf::Text createSubtitle(sf::Font const& font, std::string const& str)
{
    sf::Text text{font, str, subtitle_size};
    text.setFillColor(subtitle_color);
    return text;
}

[[nodiscard]]
inline sf::Text createValue(sf::Font const& font, std::string const& str)
{
    sf::Text text{font, str, value_size};
    text.setFillColor(value_color);
    return text;
}

inline void setOrigin(sf::Text& text, origin::Mode const mode)
{
    auto const bounds = text.getLocalBounds();
    text.setOrigin(origin::getPosition(mode, bounds.size) + bounds.position);
}

}