#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./activity_button.hpp"


struct ActivityContainer final : ui::Widget
{
    using Ptr = std::shared_ptr<ActivityContainer>;

    pez::CardOutlined background;

    explicit
    ActivityContainer(Vec2f const size_)
        : ui::Widget{size_}
        , background{ui::createBackground(size_)}
    {
        background.background.setCornerRadius(ui::background_radius + ui::margin);
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        //target.draw(background, states);
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }

    [[nodiscard]]
    ActivityButton::Ptr getButton(size_t const idx)
    {
        return getTypedChild<ActivityButton>(idx);
    }
};
