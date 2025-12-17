#pragma once

#include <SFML/Graphics.hpp>
#include "peztool/core/render.hpp"
#include "peztool/utils/render/card/card_outlined.hpp"


struct Gauge final : public sf::Drawable, sf::Transformable
{
    float padding           = 4.0f;
    float outline_thickness = 4.0f;

    Vec2f size;

    pez::CardEmpty outline;
    pez::Card      value;

    explicit
    Gauge(Vec2f const size_)
        : size{size_}
    {
        setSize(size);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        target.draw(outline, states);
        target.draw(value, states);
    }

    void setSize(Vec2f const size_)
    {
        size = size_;
        outline = pez::CardEmpty{size, size.y * 0.5f, sf::Color::White, outline_thickness};

        float const total_padding = outline_thickness + padding;
        float const height = size.y - 2.0f * total_padding;
        float const width = size.x - 2.0f * total_padding;

        value = pez::Card{{width, height}, height * 0.5f, sf::Color::White};
        value.setPosition({total_padding, total_padding});
        value.setShadowSize(0.0f);
    }

    void setPadding(float const padding_)
    {
        padding = padding_;
        setSize(size);
    }

    void setRatio(float const ratio)
    {
        float const total_padding = outline_thickness + padding;
        value.setWidth(std::clamp(ratio, 0.0f, 1.0f) * (outline.size.x - 2.0f * total_padding));
    }

};
