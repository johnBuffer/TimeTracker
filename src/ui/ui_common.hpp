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
    card.setOutlineShadowSize(0);
    card.setBackgroundShadowSize(background_shadow);
    card.background.blur_background = false;
    card.background.shadow_offset = {0.0f, 7.0f};
    card.setOutlineColor(outline_color);
    return card;
}

inline void setOrigin(sf::Text& text, origin::Mode const mode)
{
    auto const bounds = text.getLocalBounds();
    text.setOrigin(origin::getPosition(mode, bounds.size) + bounds.position);
}

}