#pragma once
#include "../quad_vertex_array.hpp"


namespace pez
{
struct Scale final : public sf::Drawable, sf::Transformable
{
    /// The offset between the label and the value line
    static float constexpr offset{5.0f};
    /// The text scale
    static float constexpr text_scale{0.35f};
    /// The width of the large ticks
    static float constexpr large_tick_width = 1.0f;
    /// The width of the small ticks
    static float constexpr small_tick_width = 0.5f;
    /// A tick every @p tick_period values
    size_t tick_period = 100;
    /// A large tick every @p tick_large_period ticks
    size_t tick_large_period = 5;
    /// The number of value ticks (Y axis)
    float value_tick = 10.0f;
    /// The size of the scale, if used with a chart, should match its size
    Vec2f size;
    /// The extreme values
    Vec2f extremes;
    /// The ticks geometry
    QuadVertexArray ticks;
    /// Whether the X ticks are enabled
    bool x_ticks = true;
    /// The labels height
    float label_height = 0.0f;

    sf::Font const* font;

    explicit
    Scale(Vec2f const size_, sf::Font const* font_ = nullptr)
        : size{size_}
        , font{font_}
    {
        if (font) {
            sf::Text value_label{*font, "8", 32};
            value_label.setScale({text_scale, text_scale});
            label_height = 3.0f * offset + value_label.getLocalBounds().size.y * text_scale;
        }
    }

    void updateGeometry(size_t const value_count, size_t const value_idx, Vec2f const extremes_)
    {
        extremes = extremes_;
        ticks.clear();
        if (value_count > 0) {
            if (x_ticks) {
                generatePeriodTicks(value_count, value_idx);
            }
            generateValueTicks();
            ticks.setAllQuadColor({255, 255, 255, 120});
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(ticks, states);

        if (font) {
            sf::Text value_label{*font, "", 32};
            foreachValueTick([&](float const value, float const y) {
                value_label.setString(std::format("{:.0f}", value));
                auto const bounds = value_label.getLocalBounds();
                if (y - bounds.size.y * text_scale - offset < 0.0f) {
                    return;
                }
                value_label.setOrigin(bounds.position + Vec2f{0.0f, bounds.size.y});
                value_label.setScale({text_scale, text_scale});
                value_label.setPosition({offset, y - offset});
                value_label.setFillColor({255, 255, 255, 50});
                target.draw(value_label, states);
            });
        }
    }

private:
    /// Generates the geometry of the ticks for the X axis
    void generatePeriodTicks(size_t const value_count, size_t const value_idx)
    {
        size_t const tick_large_value_period = tick_period * tick_large_period;
        float const  tick_space              = size.x / static_cast<float>(value_count - 1);
        for (size_t i{0}; i < value_count; ++i) {
            size_t const tick_idx = i + value_idx;
            if (tick_idx % tick_period == 0) {
                Vec2f const tick_size{
                    (tick_idx % tick_large_value_period) ? small_tick_width : large_tick_width,
                    size.y
                };
                ticks.appendAlignedRectangle(
                    tick_size,
                    Vec2f{static_cast<float>(i) * tick_space, size.y * 0.5f}
                );
            }
        }
    }

    /// Generates the geometry of the ticks for the Y axis
    void generateValueTicks()
    {
        std::vector<float> ticks_y;
        foreachValueTick([&ticks_y](float const, float const y) {
            ticks_y.push_back(y);
        });

        Vec2f const tick_size{size.x, small_tick_width};
        for (float const& y : ticks_y) {
            size_t const quad_idx = ticks.appendAlignedRectangle(
                tick_size,
                Vec2f{size.x * 0.5f, y}
            );
            ticks.setQuadColor(quad_idx, {255, 255, 255, 50});
        }
    }

    template<typename TCallback>
    void foreachValueTick(TCallback&& callback) const
    {
        // Do nothing if there is less than 2 values
        float const extreme_span = (extremes.y - extremes.x);
        if (extreme_span == 0.0f) {
            return;
        }

        float const scale = size.y / extreme_span;
        auto const getValueY = [this, scale](float const value) {
            return size.y - (value - extremes.x) * scale;
        };

        float last_y = 2.0f * size.y;
        float current_value{0.0f};
        while (true) {
            float const y = getValueY(current_value);
            if (y < 0.0f) {
                break;
            }
            if (last_y - y > label_height) {
                callback(current_value, y);
                last_y = y;
            }
            current_value += value_tick;
        }
    }
};
}
