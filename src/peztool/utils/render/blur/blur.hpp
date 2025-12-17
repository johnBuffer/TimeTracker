#pragma once
#include <SFML/Graphics.hpp>

#include "./shader.hpp"

struct Blur
{
    std::array<sf::RenderTexture, 2> textures;

    BlurShader kernel;

    size_t read_texture = 0;
    sf::Vector2i original_size;
    sf::Vector2i current_size;
    float current_scale{1.0f};

    explicit
    Blur(sf::Vector2u const render_size)
        : original_size{render_size}
        , current_size{render_size}
    {
        createTexture(textures[0], render_size);
        createTexture(textures[1], render_size);

        kernel.setRenderSize(Vec2f{render_size});
    }

    void capture(sf::Texture const& texture)
    {
        current_size = original_size;
        current_scale = 1.0f;
        read_texture = 0;
        sf::Sprite const context_sprite{texture};
        textures[read_texture].draw(context_sprite);
        textures[read_texture].display();
    }

    void scaledBlur(float const scale)
    {
        auto const& source_texture = textures[read_texture].getTexture();
        kernel.setGlobalScale(current_scale);
        kernel.setTexture(source_texture);
        kernel.setScale(scale);
        swapAndDraw(getSourceSprite(Vec2f{scale, scale}), kernel);
        current_scale *= scale;
        current_size = Vec2i{Vec2f{current_size} * scale};
    }

    void finalScale()
    {
        Vec2f const ratio = Vec2f{original_size}.componentWiseDiv(Vec2f{current_size});
        Vec2f const rescale = {std::max(ratio.x, 1.0f), std::max(ratio.y, 1.0f)};
        swapAndDraw(getSourceSprite(rescale));
    }

    [[nodiscard]]
    sf::Texture const& apply(sf::Texture const& texture)
    {
        capture(texture);
        size_t constexpr pass_count = 3;
        for (size_t i = 0; i < pass_count; ++i) {
            scaledBlur(0.5f);
        }
        for (size_t i = 0; i < pass_count; ++i) {
            scaledBlur(2.0f);
        }
        finalScale();
        return textures[read_texture].getTexture();
    }

    [[nodiscard]]
    sf::Texture const& apply(pez::RenderContext& context)
    {
        return apply(context.getTexture());
    }

private:
    void swapAndDraw(sf::Sprite const& sprite, sf::Shader const* const shader = nullptr)
    {
        textures[!read_texture].draw(sprite, shader);
        textures[!read_texture].display();
        read_texture = !read_texture;
    }

    sf::Sprite getSourceSprite(Vec2f const scale)
    {
        sf::Sprite source_sprite{textures[read_texture].getTexture()};
        source_sprite.setTextureRect({{0, original_size.y - current_size.y}, current_size});
        source_sprite.setPosition({0.0f, static_cast<float>(original_size.y)});
        source_sprite.setOrigin({0.0f, static_cast<float>(current_size.y)});
        source_sprite.setScale(scale);
        return source_sprite;
    }

    static void createTexture(sf::RenderTexture& texture, sf::Vector2u const render_size)
    {
        if (!texture.resize(render_size)) {
            std::cout << "ERROR: unable to create render texture with size " << render_size.x << ", " << render_size.y << std::endl;
        } else {
            texture.setSmooth(true);
            texture.setRepeated(false);
        }
    }
};