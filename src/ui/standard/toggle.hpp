#pragma once
#include "peztool/utils/render/card/card_outlined.hpp"
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"

#include "widget.hpp"


struct Toggle final : ui::Widget
{
    using Ptr = std::shared_ptr<Toggle>;

    static constexpr float s_radius = 15.0f;
    static constexpr float s_outline = 4.0f;
    static constexpr Vec2f s_size = {4.0f * s_radius + 2.0f * s_outline, 2.0f * (s_radius + s_outline)};
    static constexpr sf::Color color_on  = {6, 214, 160};
    static constexpr sf::Color color_off = {150, 150, 150};
    static constexpr sf::Color color_not_hover = {20, 20, 20};

    pez::CardOutlined background;

    pez::InterpolatedFloat state_x;
    pez::InterpolatedFloat state_scale;
    pez::InterpolatedData<sf::Color> state_color;
    pez::InterpolatedData<sf::Color> background_color;

    bool state = false;
    bool* bind_flag{nullptr};
    std::function<void(bool)> on_toggle_callback;
    std::function<bool()> state_update_callback;

    Toggle()
        : ui::Widget{s_size}
        , background{s_size, s_radius, s_outline, sf::Color::White}
    {
        background.outline.setShadowSize(0.0f);
        background.background.setShadowSize(16.0f);
        background.background.shadow_offset = {0.0f, 8.0f};
        background.setFillColor({50, 50, 50});

        pez::InterpolationFunction constexpr interpolation_function{pez::InterpolationFunction::EaseInOutExponential};
        float constexpr interpolation_speed{2.5f};
        state_x.setValueDirect(getX());
        state_x.setInterpolation(interpolation_function, interpolation_speed);
        state_x.useRealtime();

        state_scale.setValueDirect(1.0f);
        state_scale.setInterpolation(pez::InterpolationFunction::EaseOutBack, 2.0f);
        state_scale.useRealtime();

        state_color.setValueDirect(color_off);
        state_color.setInterpolation(interpolation_function, interpolation_speed);
        state_color.useRealtime();

        background_color.setValueDirect(color_not_hover);
        background_color.setInterpolation(interpolation_function, interpolation_speed);
        background_color.useRealtime();
    }

    void setCallback(std::function<void(bool)> callback)
    {
        on_toggle_callback = std::move(callback);
        if (bind_flag) {
            std::cout << "Unbinding flag" << std::endl;
            bind_flag = nullptr;
        }
    }

    void setUpdateCallback(std::function<bool()> callback)
    {
        state_update_callback = std::move(callback);
    }

    void updateState()
    {
        if (bind_flag) {
            dryStateChange(*bind_flag);
        } else if (state_update_callback) {
            dryStateChange(state_update_callback());
        }
    }

    void toggle()
    {
        setState(!state);
    }

    void setState(bool const new_state)
    {
        dryStateChange(new_state);
        tryUpdateBind();
    }

    void onUpdate(float const) override
    {
        updateState();
        //background.setFillColor(background_color);
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);

        sf::CircleShape toggle_status;
        float constexpr state_radius{s_radius + 0.1f};
        toggle_status.setRadius(state_radius);
        toggle_status.setOrigin({state_radius, state_radius});
        toggle_status.setPosition({state_x, s_size.y * 0.5f});
        float const s{state_scale};
        toggle_status.setScale({s, s});
        toggle_status.setFillColor(state_color);
        target.draw(toggle_status, states);
    }

    void bindTo(bool& flag)
    {
        bind_flag = &flag;
        if (on_toggle_callback) {
            std::cout << "Disabling callback" << std::endl;
            on_toggle_callback = nullptr;
        }
        if (state != flag) {
            setState(flag);
        }
    }

    void tryUpdateBind() const
    {
        if (bind_flag) {
            *bind_flag = state;
        } else if (on_toggle_callback) {
            on_toggle_callback(state);
        }
    }

    bool onClick(Vec2f const) override
    {
        toggle();
        updateHoverBackgroundColor();
        return true;
    }

    void onMouseEnter() override
    {
        //updateHoverBackgroundColor(true);
        state_scale = 1.2f;
    }

    void onMouseExit() override
    {
        //background_color = color_not_hover;
        state_scale = 1.0f;
    }

private:
    [[nodiscard]]
    float getStatusRadius() const
    {
        return s_radius;
    }

    [[nodiscard]]
    float getX() const
    {
        if (!state) {
            return getStatusRadius() + s_outline;
        }
        return s_size.x - getStatusRadius() - s_outline;
    }

    void dryStateChange(bool const new_state)
    {
        state = new_state;
        state_x = getX();
        state_color = state ? color_on : color_off;
    }

    void updateHoverBackgroundColor(bool const direct = false)
    {
        sf::Color const target_color = state ? color_off : color_on;
        if (!direct) {
            background_color = target_color;
        } else {
            background_color.setValueDirect(target_color);
        }
    }
};