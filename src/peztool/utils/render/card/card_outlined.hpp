#pragma once

#pragma once
#include "SFML/Graphics.hpp"
#include "../../../utils/vec.hpp"

#include "./card.hpp"
#include "./card_empty.hpp"

namespace pez
{

struct CardOutlined final : public sf::Drawable, public sf::Transformable
{
    Card      background;
    CardEmpty outline;

    CardOutlined() = default;

    CardOutlined(Vec2f const size_, float const corner_radius_, float const thickness, sf::Color const color)
        : background{size_, corner_radius_, color}
        , outline{size_, corner_radius_ + thickness, sf::Color::White, thickness}
    {
        setOuterSize(size_, thickness);
    }

    void setOuterSize(Vec2f const size, bool const skip_geometry_update = false)
    {
        outline.setSize(size);
        float const thickness{outline.thickness};
        background.setCornerRadius(outline.corner_radius - thickness, skip_geometry_update);
        background.setSize(outline.size - 2.0f * Vec2f{thickness, thickness}, skip_geometry_update);
    }

    void setOuterSize(Vec2f size, float thickness, bool skip_geometry = false)
    {
        outline.setSize(size, skip_geometry);
        setOutlineThickness(thickness, skip_geometry);
    }

    void setFillColor(sf::Color const color)
    {
        background.setColor(color);
    }

    void setOutlineColor(sf::Color const color)
    {
        outline.setColor(color);
    }

    /// Sets the thickness of the outline outside the background (global size = inner_size + thickness)
    void setOutlineThickness(float thickness, bool skip_geometry_update = false)
    {
        outline.setThickness(thickness, skip_geometry_update);
        background.setCornerRadius(outline.corner_radius - thickness, skip_geometry_update);
        background.setSize(outline.size - 2.0f * Vec2f{thickness, thickness}, skip_geometry_update);
        background.setPosition({thickness, thickness});
    }

    void setOutlineShadowSize(float size_, bool skip_geometry_update = false)
    {
        outline.setShadowSize(size_, skip_geometry_update);
        updateGeometry(false);
    }

    void setBackgroundShadowSize(float size_, bool skip_geometry_update = false)
    {
        background.setShadowSize(size_, skip_geometry_update);
    }

    void setRadius(float r, bool skip_geometry_update = false)
    {
        outline.corner_radius = r;
        background.corner_radius = r - getThickness();
        updateGeometry(skip_geometry_update);
    }

    [[nodiscard]]
    float getThickness() const
    {
        return outline.thickness;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(background, states);
        target.draw(outline, states);
    }

    [[nodiscard]]
    Vec2f getOutlineSize() const
    {
        return outline.size;
    }

    void updateGeometry(bool const skip)
    {
        outline.updateGeometry(skip);
        background.updateGeometry(skip);

        /*size_t const outline_quality = outline.quality;
        for (size_t i = 0; i < outline_quality + 1; i++) {
            outline.va_shadow[2 * i + 1].position += Vec2f{0.0f, 40.0f};
        }*/
    }
};

}

