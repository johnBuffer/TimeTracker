#pragma once
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "./button.hpp"


struct ButtonFancy : Button
{
    using Ptr = std::shared_ptr<ButtonFancy>;

    static constexpr sf::Color color_play          = {6, 214, 160};
    static constexpr sf::Color color_stop          = {255, 209, 102};
    static constexpr float     hover_size_increase = ui::element_spacing * 0.5f;

    pez::InterpolatedFloat background_size_offset;

    float background_radius_base{};

    bool state = false;

    explicit
    ButtonFancy(sf::Font const& font)
        : Button(sf::Color::White, "", font)
    {
        text_label.setFillColor({0, 0, 0, 150});
        background_size_offset.setInterpolation(pez::InterpolationFunction::EaseOutElastic, 1.0f);
        background_size_offset.useRealtime();
    }

    void onUpdate(float const dt) override
    {
        Vec2f const size_ = size.value_or(Vec2f{});
        float const offset = background_size_offset.getCurrentValue();
        background.setSize(size_ + Vec2f{offset, offset}, true);
        background.setOrigin(background.size * 0.5f);
        background.setCornerRadius(background_radius_base + offset);
    }

    void setOnClickCallback(std::function<void()> const callback)
    {
        Button::setOnClickCallback([this, callback]() {
            fancyOnClick();
            callback();
        });
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);
        target.draw(text_label, states);
    }

    void setSize(sf::Vector2f const& size_)
    {
        size = size_;

        background.setSize(size_);
        background.setPosition(size_ * 0.5f);
        background.setOrigin(size_ * 0.5f);
        text_label.setPosition(size_ * 0.5f);
    }

    void fancyOnClick()
    {
        background_size_offset.setValueDirect(-hover_size_increase);
        background_size_offset = 0.0f;
    }

    void setCornerRadius(float const radius)
    {
        background_radius_base = radius;
        background.setCornerRadius(radius);
    }

    void onMouseEnter() override
    {
        background_size_offset = hover_size_increase;
    }

    void onMouseExit() override
    {
        background_size_offset = 0.0f;
    }

    void setLabel(std::string const& label)
    {
        text_label.setString(label);
        ui::setOrigin(text_label, ui::origin::Mode::Center);
    }
};