#pragma once
#include "ui_configuration.hpp"

namespace ui
{

struct Data
{
    float scale = 1.0f;

    [[nodiscard]]
    float getScaled(float const x) const
    {
        return x * scale;
    }

    [[nodiscard]]
    Vec2f getScaled(Vec2f const x) const
    {
        return x * scale;
    }

    [[nodiscard]]
    float getBackgroundRadius() const
    {
        return getScaled(ui::background_radius);
    }

    [[nodiscard]]
    Vec2f getScaleVec() const
    {
        return {scale, scale};
    }
};

}