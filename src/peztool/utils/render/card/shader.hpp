#pragma once
#include <iostream>
#include <string_view>

#include "SFML/Graphics/Shader.hpp"


struct CardShader
{
    static CardShader& get()
    {
        static CardShader shader({});
        return shader;
    }

    void setRenderSize(Vec2f const size)
    {
        m_shader.setUniform("texture_size", size);
    }

    void setTexture(sf::Texture const& texture)
    {
        m_shader.setUniform("texture", texture);
    }

    operator sf::Shader const*() const
    {
        return &m_shader;
    }

private:
    explicit
    CardShader(Vec2f const render_size)
    {
        // load only the fragment shader
        if (!m_shader.loadFromMemory(s_fragment_src, sf::Shader::Type::Fragment))
        {
            std::cout << "ERROR: Failed to load card shader." << std::endl;
        }
        m_shader.setUniform("texture", sf::Shader::CurrentTexture);
        setRenderSize(render_size);
    }

    sf::Shader m_shader;

    static constexpr std::string_view s_fragment_src = R"(
    uniform sampler2D texture;
    uniform vec2      texture_size;

    void main()
    {
        vec2 screen_coord = (gl_FragCoord.xy / texture_size);
        // lookup the pixel in the texture
        vec4 pixel = texture2D(texture, screen_coord);
        // multiply it by the color
        gl_FragColor = gl_Color * pixel;
    })";
};