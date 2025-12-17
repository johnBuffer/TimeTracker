#pragma once
#include <iostream>
#include <string_view>
#include <SFML/Graphics/Shader.hpp>


struct ShadowShader
{
    static sf::Shader const* get(float const ratio, float const radius, Vec2f const scale)
    {
        static ShadowShader s_instance;
        s_instance.m_shader.setUniform("u_ratio", ratio);
        s_instance.m_shader.setUniform("u_radius", radius);
        s_instance.m_shader.setUniform("u_scale", scale);
        return &s_instance.m_shader;
    }

private:
    ShadowShader()
    {
        if (!m_shader.loadFromMemory(s_src, sf::Shader::Type::Fragment))
        {
            std::cout << "ERROR: Failed to load shadow shader." << std::endl;
        }
    }

    sf::Shader m_shader;

    static constexpr std::string_view s_src = R"(
    uniform float u_ratio;
    uniform float u_radius;
    uniform vec2  u_scale;

    float rectangleDistance(vec2 p, vec2 size, vec2 pos)
    {
        vec2 d = abs(p - pos) - size;
        return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
    }

    void main()
    {
        // Normalized pixel coordinates (from 0 to 1)
        vec2 uv = gl_Color.rg;
        uv.x *= u_ratio;

        vec2 center = vec2(0.5 * u_ratio, 0.5);
        float radius = u_radius;
        float shadow_size = 0.5 - 0.5 / u_scale.y;
        vec2 size = 0.5 * vec2(u_ratio, 1.0) / u_scale - radius;

        float carve_shadow = 0.4 * shadow_size;
        float available_from_size = min(size.x, size.y);
        float shadow_from_size = min(available_from_size, carve_shadow);
        float remaining_shadow = carve_shadow - shadow_from_size;

        float dist = rectangleDistance(uv, size - shadow_from_size, center);

        float alpha = 1.0 - smoothstep(radius - remaining_shadow, radius + shadow_size + shadow_from_size, dist);
        gl_FragColor = vec4(vec3(0.0), 0.5 * pow(alpha, 3.0));
    })";
};