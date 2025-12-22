#pragma once
#include <iostream>
#include <string_view>
#include <SFML/Graphics/Shader.hpp>
#include "peztool/utils/color_utils.hpp"


struct HatchShader
{
    Vec2f render_size;

    HatchShader()
    {
        // load only the fragment shader
        if (!m_shader.loadFromMemory(s_src, sf::Shader::Type::Fragment))
        {
            std::cout << "ERROR: Failed to load kernel shader." << std::endl;
        }
    }

    void setRenderSize(Vec2f const size)
    {
        render_size = size;
        m_shader.setUniform("render_size", size);
    }

    sf::Shader const* get() const
    {
        return &m_shader;
    }

private:
    sf::Shader m_shader;

    static constexpr std::string_view s_src = R"(
    uniform vec2 render_size;

    void main()
    {
        vec2 pos = vec2(gl_FragCoord.x / render_size.x, gl_FragCoord.y / render_size.y);
        float ratio = render_size.y / render_size.x;
        float alpha = 0.0;
        float freq = 70.0;
        if (cos(freq * pos.x - pos.y * freq * ratio) > 0.0) {
            alpha = 0.02;
        }
        gl_FragColor = vec4(vec3(1.0), alpha);
    })";
};