#pragma once
#include "peztool/utils/render/card/card_outlined.hpp"

#include "./ui_data.hpp"
#include "standard/widget.hpp"

namespace ui
{
[[nodiscard]]
inline pez::CardOutlined createBackground(Vec2f const size)
{
    auto const& data = pez::Singleton<ui::Data>::get();
    pez::CardOutlined card{size, background_radius, outline_thickness, background_color};
    card.setOutlineShadowSize(0);
    card.setBackgroundShadowSize(data.getScaled(ui::background_shadow));
    card.background.blur_background = false;
    card.background.shadow_offset = {0.0f, data.getScaled(7.0f)};
    card.setOutlineColor(outline_color);
    return card;
}

inline void setOrigin(sf::Text& text, origin::Mode const mode)
{
    auto const bounds = text.getLocalBounds();
    text.setOrigin(origin::getPosition(mode, bounds.size) + bounds.position);
}

}