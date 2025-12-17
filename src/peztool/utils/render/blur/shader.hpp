#pragma once
#include <iostream>
#include <string_view>
#include "SFML/Graphics/Shader.hpp"


struct BlurShader
{
    BlurShader()
    {
        // load only the fragment shader
        if (!m_shader.loadFromMemory(s_src, sf::Shader::Type::Fragment))
        {
            std::cout << "ERROR: Failed to load kernel shader." << std::endl;
        }

        setRadius(4.5f);
    }

    void setRenderSize(Vec2f const size)
    {
        m_shader.setUniform("texture_size", size);
    }

    void setTexture(sf::Texture const& texture)
    {
        m_shader.setUniform("texture", texture);
    }

    void setScale(float const scale)
    {
        m_shader.setUniform("scale", scale);
    }

    void setGlobalScale(float const global_scale)
    {
        m_shader.setUniform("global_scale", global_scale);
    }

    void setRadius(float const radius)
    {
        m_shader.setUniform("blur_radius", radius);
    }

    operator sf::Shader*()
    {
        return &m_shader;
    }

private:
    sf::Shader m_shader;

    static constexpr std::string_view s_src = R"(
    uniform sampler2D texture;
    uniform vec2  texture_size;
    uniform float scale;
    uniform float global_scale;
    uniform float blur_radius;

    vec4 smartSample(sampler2D texture, vec2 uv, float weight)
    {
        vec2 safety_margin = vec2(global_scale) - 4.0 / texture_size;
        uv = min(safety_margin, uv);
        return vec4(texture2D(texture, uv).rgb * weight, weight);
    }

    void main()
    {
        vec2 off_x = vec2(blur_radius / texture_size.x, 0.0);
        vec2 off_y = vec2(0.0, blur_radius / texture_size.y);

        vec2 screen_coord = (gl_FragCoord.xy / texture_size);
        vec2 uv = screen_coord / scale;

        vec4 color = vec4(0.0);
        color += smartSample(texture, uv                , 4.0);
        color += smartSample(texture, uv - off_x        , 2.0);
        color += smartSample(texture, uv + off_x        , 2.0);
        color += smartSample(texture, uv - off_y        , 2.0);
        color += smartSample(texture, uv + off_y        , 2.0);
        color += smartSample(texture, uv - off_x - off_y, 1.0);
        color += smartSample(texture, uv - off_x + off_y, 1.0);
        color += smartSample(texture, uv + off_x - off_y, 1.0);
        color += smartSample(texture, uv + off_x + off_y, 1.0);

        gl_FragColor = vec4(color.rgb / color.a, 1.0);
    })";
};