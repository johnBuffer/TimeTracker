#pragma once
#include "../vec.hpp"
#include "./quad_vertex_array.hpp"

namespace pez
{
struct BackgroundGrid final : public sf::Drawable, public sf::Transformable
{
    QuadVertexArray va;

    float small_width = 1.0f;
    float large_width = 2.0f;

    Vec2i size;
    Vec2u ticks;

    BackgroundGrid() = default;

    BackgroundGrid(Vec2i const size_, uint32_t const small_tick_period, uint32_t const large_tick_period)
        : size{size_}
        , ticks{small_tick_period, large_tick_period}
    {
        updateGeometry();
    }

    void setThickness(float const small, float const large)
    {
        small_width = small;
        large_width = large;
        updateGeometry();
    }

    void updateGeometry()
    {
        uint32_t const small_tick{ticks.x};
        uint32_t const large_tick{ticks.y};
        uint32_t const vertical_small_count  {size.x / small_tick};
        uint32_t const horizontal_small_count{size.y / small_tick};
        uint32_t const vertical_large_count  {size.x / large_tick};
        uint32_t const horizontal_large_count{size.y / large_tick};

        va.resize(vertical_small_count + horizontal_small_count + vertical_large_count + horizontal_large_count + 4);

        float const half_small{small_width * 0.5f};
        uint32_t global_index{0};

        // Vertical small
        for (uint32_t i{1}; i < vertical_small_count; ++i) {
            uint32_t const idx{i + global_index};
            auto const x = static_cast<float>(i * small_tick);
            va.setVertex0Position(idx, {x - half_small,                       0.0f});
            va.setVertex1Position(idx, {x + half_small,                       0.0f});
            va.setVertex2Position(idx, {x + half_small, static_cast<float>(size.y)});
            va.setVertex3Position(idx, {x - half_small, static_cast<float>(size.y)});
        }
        global_index += vertical_small_count;

        // Horizontal small
        for (uint32_t i{1}; i < horizontal_small_count; ++i) {
            uint32_t const idx{i + global_index};
            auto const y = static_cast<float>(i * small_tick);
            va.setVertex0Position(idx, {0.0f                      , y - half_small});
            va.setVertex1Position(idx, {0.0f                      , y + half_small});
            va.setVertex2Position(idx, {static_cast<float>(size.x), y + half_small});
            va.setVertex3Position(idx, {static_cast<float>(size.x), y - half_small});
        }
        global_index += horizontal_small_count;

        float const half_large{large_width * 0.5f};
        // Vertical large
        for (uint32_t i{1}; i < vertical_large_count; ++i) {
            uint32_t const idx{i + global_index};
            auto const x = static_cast<float>(i * large_tick);
            va.setVertex0Position(idx, {x - half_large,                       0.0f});
            va.setVertex1Position(idx, {x + half_large,                       0.0f});
            va.setVertex2Position(idx, {x + half_large, static_cast<float>(size.y)});
            va.setVertex3Position(idx, {x - half_large, static_cast<float>(size.y)});
        }
        global_index += vertical_large_count - 1;

        // Horizontal large
        for (uint32_t i{1}; i < horizontal_large_count; ++i) {
            uint32_t const idx{i + global_index};
            auto const y = static_cast<float>(i * large_tick);
            va.setVertex0Position(idx, {0.0f                      , y - half_large});
            va.setVertex1Position(idx, {0.0f                      , y + half_large});
            va.setVertex2Position(idx, {static_cast<float>(size.x), y + half_large});
            va.setVertex3Position(idx, {static_cast<float>(size.x), y - half_large});
        }
    }

    void setColor(sf::Color const color)
    {
        va.setAllQuadColor(color);
    }

    void render(RenderContext& context) const
    {
        context.draw(va.asVertexArray(), context.getWorldLayerID());
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(va.asVertexArray(), states);
    }
};
}