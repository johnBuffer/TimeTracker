#pragma once
#include "SFML/Graphics.hpp"
#include "../../vec.hpp"
#include "./shader.hpp"
#include "./utils.hpp"
#include "../quad_vertex_array.hpp"
#include "./shadow_shader.hpp"


namespace pez
{

struct Card : public sf::Drawable, public sf::Transformable
{
    sf::VertexArray va;
    QuadVertexArray va_shadow;

    Vec2f     size;
    float     corner_radius = 0.0f;
    sf::Color color;

    float     shadow_size   = 8.0f;
    Vec2f     shadow_offset = {0.0f, 0.0f};
    sf::Color shadow_color = sf::Color::Black;

    uint32_t quality = 64;
    bool blur_background = false;

    Card()
        : va{sf::PrimitiveType::TriangleFan}
        , va_shadow{1}
    {}

    Card(Vec2f const size_, float const corner_radius_, sf::Color const color_)
        : va{sf::PrimitiveType::TriangleFan}
        , va_shadow{1}
        , size{size_}
        , corner_radius{corner_radius_}
        , color{color_}
    {
        updateGeometry(false);
    }

    void setColor(sf::Color const color_)
    {
        color = color_;
        uint64_t const vertex_count = va.getVertexCount();
        for (uint64_t i{vertex_count}; i--;) {
            va[i].color = color;
        }
    }

    void setCornerRadius(float radius, bool skip_geometry_update = false)
    {
        corner_radius = radius;
        updateGeometry(skip_geometry_update);
    }

    void setShape(Vec2f size_, float corner_radius_, bool skip_geometry_update = false)
    {
        corner_radius = corner_radius_;
        size = size_;
        updateGeometry(skip_geometry_update);
    }

    void updateGeometry(bool skip)
    {
        if (skip) {
            return;
        }
        generateGeometry(va);
        generateGeometryShadow(va_shadow);
        setColor(color);
    }

    void setShadowSize(float size_, bool skip_geometry_update = false)
    {
        shadow_size = size_;
        updateGeometry(skip_geometry_update);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        if (shadow_size > 0.0f) {
            sf::RenderStates states_shadow = states;
            states_shadow.texture = nullptr;
            Vec2f const quad_size = getShadowQuadSize();
            states_shadow.transform.translate(shadow_offset);
            states_shadow.transform.translate(size * 0.5f);
            states_shadow.shader = ShadowShader::get(
                quad_size.x / quad_size.y,
                corner_radius / quad_size.y,
                quad_size.componentWiseDiv(size),
                shadow_color);
            target.draw(va_shadow, states_shadow);
        }

        if (blur_background) {
            states.shader = CardShader::get();
        } else {
            states.texture = m_texture;
        }
        target.draw(va, states);
    }

    void setWidth(float width, bool skip_geometry_update = false)
    {
        size.x = width;
        updateGeometry(skip_geometry_update);
    }

    void setHeight(float height, bool skip_geometry_update = false)
    {
        size.y = height;
        updateGeometry(skip_geometry_update);
    }

    void setSize(Vec2f size_, bool skip_geometry_update = false)
    {
        size = size_;
        updateGeometry(skip_geometry_update);
    }

    void generateGeometry(sf::VertexArray& vertex_array)
    {
        float const radius = std::min(std::min(corner_radius, size.x * 0.5f), size.y * 0.5f);
        CardGeometryGenerator generator;
        generator.quality = quality;
        generator.size = size;
        generator.radius = radius;
        generator.start = 1;

        uint64_t const vertex_count = generator.getVertexCount() + 1;
        vertex_array.resize(vertex_count);
        vertex_array[0].position = size * 0.5f;

        generator.generateVertex(&vertex_array);
    }

    void generateGeometryShadow(QuadVertexArray& vertex_array)
    {
        Vec2f const quad_size = getShadowQuadSize();
        vertex_array.createAlignedRectangle(0, quad_size, {0.0f, 0.0f});
        vertex_array.setVertex0Color(0, {0, 0, 0});
        vertex_array.setVertex1Color(0, {255, 0, 0});
        vertex_array.setVertex2Color(0, {255, 255, 0});
        vertex_array.setVertex3Color(0, {0, 255, 0});
    }

    [[nodiscard]]
    Vec2f getShadowQuadSize() const
    {
        return size + 2.0f * Vec2f{shadow_size, shadow_size};
    }

    void setTexture(sf::Texture const* texture_, float texture_scale = 1.0f)
    {
        m_texture = texture_;
        if (m_texture) {
            auto const texture_size = Vec2f{m_texture->getSize()};
            Vec2f const scale = texture_size.componentWiseDiv(size);
            size_t const vertex_count = va.getVertexCount();
            for (size_t i{0}; i < vertex_count; ++i) {
                Vec2f const position = va[i].position;
                va[i].texCoords = position.componentWiseMul(scale * texture_scale);
            }
        }
    }

private:
    sf::Texture const* m_texture = nullptr;
};
}
