#pragma once
#include <cstdint>
#include <cmath>
#include "SFML/Graphics/Color.hpp"
#include "./vec.hpp"


namespace pez
{

template<typename TVec4>
TVec4 getVec4(sf::Color const color)
{
    return {static_cast<float>(color.r),
            static_cast<float>(color.g),
            static_cast<float>(color.b),
            static_cast<float>(color.a)};
}

template<typename TVec3>
TVec3 getVec3(sf::Color const color)
{
    return {static_cast<float>(color.r),
            static_cast<float>(color.g),
            static_cast<float>(color.b)};
}

template<typename TVec4>
sf::Color getColor(TVec4 const vec)
{
    return {static_cast<uint8_t>(std::clamp(vec.x, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.y, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.z, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.w, 0.0f, 255.0f)),
    };
}

template<typename TVec3>
sf::Color getColorVec3(TVec3 const vec)
{
    return {static_cast<uint8_t>(std::clamp(vec.x, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.y, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.z, 0.0f, 255.0f))};
}

inline sf::Color getMix(sf::Color const color_from, sf::Color const color_to, float const ratio)
{
    auto const v_from = getVec4<Vec4f>(color_from);
    auto const v_to = getVec4<Vec4f>(color_to);
    return getColor(v_from * (1.0f - ratio) + v_to * ratio);
}

inline sf::Color setLuminance(sf::Color const color, float const ratio)
{
    auto const v = getVec4<Vec4f>(color);
    sf::Color res = getColorVec3(v * ratio);
    res.a = color.a;
    return res;
}

constexpr sf::Color setAlpha(sf::Color color, uint8_t alpha)
{
    return {color.r, color.g, color.b, alpha};
}

}