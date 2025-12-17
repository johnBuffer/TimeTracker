#pragma once
#include "peztool/utils/render/card/card_outlined.hpp"

#include "../ui_configuration.hpp"
#include "../ui_common.hpp"

#include "./widget.hpp"


struct Button : ui::Widget
{
    using Ptr = std::shared_ptr<Button>;

    float radius = 15.0f;
    float outline = 4.0f;

    pez::Card background;

    sf::Text text_label;

    bool locked = false;

    std::function<void()> on_click_callback;

    Button(sf::Color const color, std::string const& label, sf::Font const& font)
        : background{{}, radius, color}
        , text_label{font, label, 24}
    {
        float constexpr label_margin = 20.0f;
        text_label.setFillColor(ui::subtitle_color);
        ui::setOrigin(text_label, ui::origin::Mode::Center);
        size = {text_label.getLocalBounds().size + 2.0f * Vec2f{label_margin, label_margin}};
        text_label.setPosition(*size * 0.5f);

        background.setShadowSize(ui::background_shadow * 0.5f);
        background.setSize(*size);
    }

    void setOnClickCallback(std::function<void()> callback)
    {
        on_click_callback = std::move(callback);
    }

    void setLabelOffset(float const offset)
    {
        text_label.setPosition(*size * 0.5f + Vec2f{0.0f, offset});
    }

    void onUpdate(float const) override
    {

    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);
        target.draw(text_label, states);
    }

    bool onClick(Vec2f const) override
    {
        if (on_click_callback) {
            on_click_callback();
        }
        return true;
    }

    void onMouseEnter() override
    {
        std::cout << "Button::onMouseEnter" << std::endl;
    }

    void onMouseExit() override
    {
        std::cout << "Button::onMouseExit" << std::endl;
    }
};