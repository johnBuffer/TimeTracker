#pragma once
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "peztool/utils/render/card/card_outlined.hpp"
#include "./widget.hpp"


struct Slider final : public ui::Widget
{
    using Ptr = std::shared_ptr<Slider>;

    static constexpr float s_radius = 15.0f;
    static constexpr float s_outline = 4.0f;
    static constexpr float s_total_radius = s_radius + s_outline;
    static constexpr float s_height = 2.0f * s_total_radius;
    static constexpr sf::Color color_hover = {250, 250, 250};
    static constexpr sf::Color color_not_hover = {150, 150, 150};

    float width_span;
    int32_t tick_count;
    int32_t current_tick{0};
    float tick_length{};
    bool on_cursor{false};
    pez::InterpolatedFloat cursor_scale;

    pez::CardOutlined cursor;
    pez::InterpolatedData<sf::Color> background_color;

    std::function<void(int32_t)> on_tick_change;

    explicit
    Slider(float const width, int32_t const tick_count_)
        : ui::Widget{{width, s_height}}
        , width_span{width - 2.0f * s_total_radius}
        , tick_count{tick_count_}
        , tick_length{width_span / static_cast<float>(tick_count - 1)}
        , cursor{2.0f * Vec2f{s_total_radius, s_total_radius}, s_radius, s_outline, color_not_hover}
    {
        cursor.setOrigin({s_total_radius, s_total_radius});
        updateCursorPosition();

        pez::InterpolationFunction constexpr interpolation_function{pez::InterpolationFunction::EaseInOutExponential};
        float constexpr interpolation_speed{2.5f};
        background_color.setValueDirect(color_not_hover);
        background_color.setInterpolation(interpolation_function, interpolation_speed);
        background_color.useRealtime();

        cursor_scale.setInterpolation(pez::InterpolationFunction::EaseOutElastic, 2.0f);
        cursor_scale.setValueDirect(1.0f);
        cursor_scale.useRealtime();
    }

    void onUpdate(float) override
    {
        cursor.setScale({cursor_scale, cursor_scale});
        cursor.setFillColor(background_color);
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        float constexpr slider_height = 4.0f;
        sf::RectangleShape slider{{width_span, slider_height}};
        slider.setOrigin({0.0f, slider_height * 0.5f});
        slider.setPosition({s_total_radius, size->y * 0.5f});
        target.draw(slider, states);

        target.draw(cursor, states);
    }

    bool onClick(Vec2f const position) override
    {
        return on_cursor;
    }

    void onMouseMove(Vec2f const position) override
    {
        if (isClicked()) {
            int32_t const tick = getTick(position.x);
            if (tick != current_tick) {
                setTick(tick);
            }
        } else {
            float const cursor_position = getCurrentX();
            bool const on_cursor_last = on_cursor;
            on_cursor = std::abs(position.x - cursor_position) < s_total_radius;

            if (on_cursor && !on_cursor_last) {
                background_color.setValueDirect(color_hover);
            } else if (!on_cursor && on_cursor_last) {
                background_color = color_not_hover;
            }
        }
    }

    void setTick(int32_t const tick)
    {
        current_tick = tick;
        updateCursorPosition();
        if (on_tick_change) {
            on_tick_change(current_tick);
        }
        cursor_scale.setValueDirect(0.75f);
        cursor_scale = 1.0f;
    }

    void updateCursorPosition()
    {
        cursor.setPosition({getCurrentX(), size->y * 0.5f});
    }

    void onMouseExit() override
    {
        background_color = color_not_hover;
        on_cursor = false;
    }

    [[nodiscard]]
    int32_t getSelectedTick() const
    {
        return current_tick;
    }

    void setOnTickChangeCallback(std::function<void(int32_t)> callback)
    {
        on_tick_change = std::move(callback);
    }

    [[nodiscard]]
    float getTickRatio(int32_t const tick) const
    {
        return static_cast<float>(tick) / static_cast<float>(tick_count - 1);
    }

private:
    [[nodiscard]]
    float getCurrentX() const
    {
        float constexpr left_padding = s_total_radius;
        return left_padding + static_cast<float>(current_tick) * tick_length;
    }

    [[nodiscard]]
    int32_t getTick(float const position_x) const
    {
        float const offset_position_x = position_x + tick_length * 0.5f - s_total_radius;
        float const clamped_position_x = std::clamp(offset_position_x, 0.0f, width_span);
        return static_cast<int32_t>(clamped_position_x / tick_length);
    }
};
