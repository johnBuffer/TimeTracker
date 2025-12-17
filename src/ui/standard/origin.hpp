#pragma once

namespace ui::origin
{

/// Describes common origins
enum class Mode
{
    Center,
    LeftCenter,
    LeftTop,
    LeftBottom,
    RightCenter,
    RightTop,
    RightBottom,
    TopCenter,
    TopLeft,
    TopRight,
    BottomCenter,
    BottomLeft,
    BottomRight,
};

inline Vec2f getPosition(Mode const mode, Vec2f const size)
{
    switch (mode) {
        case Mode::Center:
            return size * 0.5f;
        case Mode::LeftCenter:
            return {0.0f, size.y * 0.5f};
        case Mode::LeftTop:
        case Mode::TopLeft:
            return {0.0f, 0.0f};
        case Mode::LeftBottom:
        case Mode::BottomLeft:
            return {0.0f, size.y};
        case Mode::RightCenter:
            return {size.x, size.y * 0.5f};
        case Mode::RightTop:
        case Mode::TopRight:
            return {size.x, 0.0f};
        case Mode::RightBottom:
        case Mode::BottomRight:
            return size;
        case Mode::TopCenter:
            return {size.x * 0.5f, 0.0f};
        case Mode::BottomCenter:
            return {size.x * 0.5f, size.y};
        default:
            return {};
    }
}

}