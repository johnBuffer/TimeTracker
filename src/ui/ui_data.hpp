#pragma once

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
    Vec2f getScaleVec() const
    {
        return {scale, scale};
    }
};

}