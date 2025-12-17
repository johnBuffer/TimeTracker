#pragma once
#include "configuration.hpp"
#include "peztool/utils/vec.hpp"

float constexpr meters(float const m)
{
    return m * sim_const::meter_to_pixel;
}

float constexpr centimeters(float const cm)
{
    return meters(cm / float{100});
}

float constexpr millimeters(float const mm)
{
    return meters(mm / float{1000});
}

Vec2f constexpr meters(Vec2f const v)
{
    return {meters(v.x), meters(v.y)};
}

Vec2f constexpr centimeters(Vec2f const v)
{
    return {centimeters(v.x), centimeters(v.y)};
}

Vec2f constexpr millimeters(Vec2f const v)
{
    return {millimeters(v.x), millimeters(v.y)};
}