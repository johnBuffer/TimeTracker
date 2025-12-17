#pragma once
#include "../vec.hpp"

struct Tracer
{
    sf::Color                color = sf::Color::White;
    std::vector<Vec2f>       points;
    sf::VertexArray          va_line;

    float width_start = 4.0f;
    float offset_x = 0.0f;

    explicit
    Tracer()
        : va_line{sf::PrimitiveType::TriangleStrip}
    {}

    void setColor(sf::Color c)
    {
        color = c;
    }

    void clear()
    {
        points.clear();
    }

    void addPoint(Vec2f const pt)
    {
        points.push_back(pt);
    }

    void render(pez::RenderContext& context)
    {
        va_line.resize(2 * points.size() - 2);

        if (points.size() > 1) {
            Vec2f last = points[0];
            uint32_t i{0};
            for (auto const& pt: points) {
                if (i > 0) {
                    Vec2f const current = pt;
                    Vec2f const d = current - last;
                    Vec2f const n = pez::normalize(pez::normal(d));
                    float const w = width_start;
                    va_line[2 * (i - 1) + 0].position = current + w * n;
                    va_line[2 * (i - 1) + 1].position = current - w * n;
                    va_line[2 * (i - 1) + 0].color = color;
                    va_line[2 * (i - 1) + 1].color = color;
                    last = current;
                }
                ++i;
            }

            sf::Transform tf;
            tf.translate({offset_x, 0.0f});
            context.getWorldLayer().draw(va_line, tf);
        }
    }
};
