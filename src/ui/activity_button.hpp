#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct ActivityButton final : ui::Widget
{
    using Ptr = std::shared_ptr<ActivityButton>;

    sf::Font const& font;

    pez::CardOutlined background;

    pez::InterpolatedFloat highlight_offset;

    explicit
    ActivityButton(sf::Font const& font_, Vec2f const size_)
        : ui::Widget{size_}
        , font{font_}
        , background{ui::createBackground(size_)}
    {
        highlight_offset.setInterpolationSpeed(5.0f);
        highlight_offset.setValueDirect(0.0f);
    }

    void onUpdate(float const dt) override
    {
        background.setPosition({0.0f, highlight_offset});
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }

    void onMouseEnter() override
    {
        highlight_offset = -ui::margin * 0.5f;
    }

    void onMouseExit() override
    {
        highlight_offset = 0.0f;
    }
};
