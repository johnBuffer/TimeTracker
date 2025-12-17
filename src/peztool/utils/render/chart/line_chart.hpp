#pragma once
#include <SFML/Graphics.hpp>

#include "../../vec.hpp"
#include "../../math.hpp"
#include "../../racc.hpp"

namespace pez
{
struct LineChart final : public sf::Drawable, public sf::Transformable
{
    // Data
    mutable RAccBase<float> values;
    Vec2f           extremes;
    sf::Vector2f    size;
    sf::VertexArray va_area;
    sf::VertexArray va_line;

    uint32_t values_added = 0;

    sf::Color color;

    float line_thickness = 2.0f;

    bool draw_area = true;
    bool draw_line = true;

    explicit
    LineChart(sf::Vector2f const size_)
        : values(100)
        , size{size_}
        , va_area{sf::PrimitiveType::TriangleStrip}
        , va_line{sf::PrimitiveType::TriangleStrip}
        , color{sf::Color::Red}
    {}

    void setSize(Vec2f const size_)
    {
        size = size_;
    }

    void clear()
    {
        values.clear();
        va_line.clear();
        va_area.clear();
        values_added = 0;
        extremes = {0.0f, 0.0f};
    }

    void addValue(float const new_value)
    {
        ++values_added;

        values.add(new_value);

        if (new_value < extremes.x) {
            extremes.x = new_value;
        }
        if (new_value > extremes.y) {
            extremes.y = new_value;
        }

        updateGeometry();
    }

    [[nodiscard]]
    Vec2f getPoint(uint32_t const i) const
    {
        return va_area[2 * i].position;
    }

    [[nodiscard]]
    static Vec2f getIntersection(Vec2f const p1, Vec2f const n1, Vec2f const p2, Vec2f const n2)
    {
        Vec2f const p1End = p1 + n1;
        Vec2f const p2End = p2 + n2;

        float const m1 = (p1End.y - p1.y) / (p1End.x - p1.x);
        float const m2 = (p2End.y - p2.y) / (p2End.x - p2.x);

        float const b1 = p1.y - m1 * p1.x;
        float const b2 = p2.y - m2 * p2.x;

        float const px = (b2 - b1) / (m1 - m2);
        float const py = m1 * px + b1;

        return {px, py};
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        if (draw_area) {
            target.draw(va_area, states);
        }

        if (draw_line) {
            target.draw(va_line, states);
        }
    }

    void updateGeometry()
    {
        uint32_t const count = values.getCount();
        float const alpha_max_height = 1.0f / std::max(std::abs(extremes.x), std::abs(extremes.y));
        float const dx = size.x / float(count - 1);

        va_area.resize(2 * count);
        values.foreach([&](uint32_t i, float v) {
            float const normalized_x = static_cast<float>(i) * dx;
            float const scaled_y     = getScaledY(v);
            va_area[2 * i].position = {normalized_x, scaled_y};

            float const alpha_ratio = std::abs(v) * alpha_max_height;
            auto const  alpha       = static_cast<uint8_t>(std::min(1.0f, 2.0f * alpha_ratio) * 150.0f);
            sf::Color const area_color{color.r, color.g, color.b, alpha};
            va_area[2 * i].color = area_color;

            va_area[2 * i + 1].position = {normalized_x, getScaledY(0.0f)};
            va_area[2 * i + 1].color = {color.r, color.g, color.b, 0};
        });

        if (draw_line) {
            va_line.resize(2 * count);
            if (count > 1) {
                Vec2f last_v{pez::normalize(getPoint(1) - getPoint(0))};
                Vec2f last_n{pez::normal(last_v)};

                va_line[0].position = getPoint(0) + Vec2f{0, line_thickness};
                va_line[0].color = color;
                va_line[1].position = getPoint(0) - Vec2f{0, line_thickness};
                va_line[1].color = color;

                for (uint32_t i{2}; i < count; ++i) {
                    Vec2f const v = pez::normalize(getPoint(i) - getPoint(i - 1));
                    Vec2f const n = pez::normal(v);

                    float const dot = pez::dot(last_v, v);

                    auto const computeGeometry = [&](uint32_t const idx, float const normal_direction, float const dot_result) {
                        float constexpr threshold = 0.9f;
                        float const     normal    = line_thickness * normal_direction;

                        if (dot < threshold) {
                            Vec2f const pt_1 = getPoint(i) + n * normal;
                            Vec2f const pt_2 = getPoint(i - 1) + last_n * normal;
                            va_line[idx].position = getIntersection(pt_1, v, pt_2, last_v);
                            va_line[idx].color = color;
                        } else {
                            Vec2f const pt_1 = getPoint(i - 1) + n * normal;
                            va_line[idx].position = pt_1;
                            va_line[idx].color = color;
                        }
                    };

                    computeGeometry(2 * (i - 1)    ,  1.0f, dot);
                    computeGeometry(2 * (i - 1) + 1, -1.0f, dot);

                    last_v = v;
                    last_n = n;
                }

                size_t const last_idx = count - 1;
                va_line[2 * last_idx].position = getPoint(count - 1) + line_thickness * Vec2f{0, 1};
                va_line[2 * last_idx].color = color;
                va_line[2 * last_idx + 1].position = getPoint(count - 1) - line_thickness * Vec2f{0, 1};
                va_line[2 * last_idx + 1].color = color;
            }
        }
    }

    [[nodiscard]]
    uint32_t getGlobalValueIndex(uint32_t data_buffer_idx) const
    {
        return data_buffer_idx + ((values_added < values.max_values_count) ? 0 : (values_added - values.max_values_count));
    }

    [[nodiscard]]
    float getScaledY(float y) const
    {
        float const width = extremes.y - extremes.x;
        float const normalized_y = (y - extremes.x) / width;
        float const scaled_y = normalized_y * size.y;
        return size.y - scaled_y;
    }

    void setColor(sf::Color c)
    {
        color = c;
    }
};
}
