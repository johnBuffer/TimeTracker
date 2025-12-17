#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct TimeBar final : ui::Widget
{
    using Ptr = std::shared_ptr<TimeBar>;

    sf::Font const& font;

    pez::CardOutlined background;

    explicit
    TimeBar(sf::Font const& font_, Vec2f const size_)
        : ui::Widget{size_}
        , font{font_}
        , background{ui::createBackground(size_)}
    {

    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }
};
