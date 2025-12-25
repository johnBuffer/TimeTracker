#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./ui_data.hpp"


struct Container final : ui::Widget
{
    using Ptr = std::shared_ptr<Container>;

    pez::CardOutlined background;

    explicit
    Container(Vec2f const size_)
        : ui::Widget{size_}
        , background{ui::createBackground(size_)}
    {
        auto const& data = pez::Singleton<ui::Data>::get();
        background.background.setCornerRadius(data.getScaled(ui::background_radius + ui::margin));
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
