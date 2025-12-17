#pragma once
#pragma once
#include "peztool/utils/render/card/card_outlined.hpp"

#include "widget.hpp"
#include "../ui_configuration.hpp"


struct RadioButton final : ui::Widget
{
    using Ptr = std::shared_ptr<RadioButton>;

    static constexpr float s_radius = 15.0f;
    static constexpr float s_outline = 4.0f;
    static constexpr Vec2f s_size = 2.0f * Vec2f{s_radius + s_outline, s_radius + s_outline};
    static constexpr sf::Color color_on  = {6, 214, 160};
    static constexpr sf::Color color_off = {150, 150, 150};
    static constexpr sf::Color color_hover = {150, 150, 150, 100};
    static constexpr sf::Color color_not_hover = {20, 20, 20, 100};

    pez::CardOutlined background;

    pez::InterpolatedData<sf::Color> background_color;
    pez::InterpolatedFloat state_scale;

    bool state = false;

    std::function<void()> on_select_callback;

    RadioButton()
        : ui::Widget{s_size}
        , background{s_size, s_radius, s_outline, sf::Color::White}
    {
        pez::InterpolationFunction constexpr interpolation_function{pez::InterpolationFunction::EaseInOutExponential};
        float constexpr interpolation_speed{2.5f};

        background_color.setValueDirect(color_not_hover);
        background_color.setInterpolation(interpolation_function, interpolation_speed);
        background_color.useRealtime();

        state_scale.setValueDirect(0.0f);
        state_scale.setInterpolation(interpolation_function, interpolation_speed);
        state_scale.useRealtime();
    }

    void setSelectCallback(std::function<void()> callback)
    {
        on_select_callback = std::move(callback);
    }

    void setState(bool const new_state)
    {
        state = new_state;
        if (state) {
            state_scale = 1.0f;
            updateHoverBackgroundColor();
        } else {
            state_scale = 0.0f;
            background_color = color_not_hover;
        }
    }

    void onUpdate(float const) override
    {
        background.setFillColor(background_color);
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);

        float constexpr state_padding = 4.0f;
        float const state_radius = (s_radius - state_padding) * state_scale;
        sf::CircleShape toggle_status;
        toggle_status.setRadius(state_radius);
        toggle_status.setOrigin({state_radius, state_radius});
        toggle_status.setPosition({s_radius + s_outline, s_radius + s_outline});
        toggle_status.setFillColor(color_on);
        target.draw(toggle_status, states);
    }

    bool onClick(Vec2f const) override
    {
        on_select_callback();
        return true;
    }

    void onMouseEnter() override
    {
        updateHoverBackgroundColor(true);
    }

    void onMouseExit() override
    {
        background_color = color_not_hover;
    }

private:
    void updateHoverBackgroundColor(bool const direct = false)
    {
        sf::Color const target_color = pez::setAlpha(state ? color_off : color_on, 100);
        if (!direct) {
            background_color = target_color;
        } else {
            background_color.setValueDirect(target_color);
        }
    }
};