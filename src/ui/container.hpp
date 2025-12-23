#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./activity_button.hpp"


struct Container final : ui::Widget
{
    using Ptr = std::shared_ptr<Container>;

    pez::CardOutlined background;

    explicit
    Container(Vec2f const size_)
        : ui::Widget{size_}
        , background{ui::createBackground(size_)}
    {
        background.background.setCornerRadius(ui::background_radius + ui::margin);
        background.setFillColor({150, 150, 150});
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
