#pragma once
#include <chrono>

#include "../../peztool.hpp"
#include "../../utils/vec.hpp"
#include "../color_utils.hpp"
#include "./interpolable.hpp"


namespace pez
{

/// Base class for standard InterpolatedData
template<typename TData>
struct InterpolatedData final : public Interpolable
{
public:
    InterpolatedData(TData const& value, InterpolationFunction const function, float const speed)
    {
        setValueDirect(value);
        setInterpolationFunction(function);
        setInterpolationSpeed(speed);
    }

    InterpolatedData(TData const& value, InterpolationFunction const function)
        : InterpolatedData(value, function, 1.0f)
    {
    }

    InterpolatedData(TData const& value, float const speed)
        : InterpolatedData(value, InterpolationFunction::EaseInOutQuint, speed)
    {
    }

    explicit
    InterpolatedData(TData const& value)
        : InterpolatedData(value, InterpolationFunction::EaseInOutQuint, 1.0f)
    {
    }

    InterpolatedData()
        : InterpolatedData({}, InterpolationFunction::EaseInOutQuint, 1.0f)
    {}

    ~InterpolatedData() override = default;

    void useRealtime()
    {
        m_use_realtime = true;
    }

    /// Sets a new target for the value
    void setValue(TData const& value)
    {
        if (value == m_target_value) {
            return;
        }
        m_start_value  = getCurrentValue();
        m_target_value = value;
        m_delta        = m_target_value - m_start_value;
        reset();
    }

    /// Instantly sets the current value to the provided one
    void setValueDirect(TData const& value)
    {
        m_start_value  = value;
        m_target_value = value;
        m_delta        = {};
        setDone();
    }

    /// Adds the provided offset to the current value
    void offsetValue(TData const& offset)
    {
        setValue(getCurrentValue() + offset);
    }

    /// Instantly adds the provided offset to the current value
    void offsetValueDirect(TData const& offset)
    {
        setValueDirect(getCurrentValue() + offset);
    }

    /// Casts the value to the underlying type by returning the current value
    operator TData() const
    {
        return getCurrentValue();
    }

    [[nodiscard]]
    TData getCurrentValue() const
    {
        if (!isDone()) {
            return m_start_value + m_delta * getValueRatio();
        }
        return m_target_value;
    }

    InterpolatedData& operator=(TData const& value)
    {
        setValue(value);
        return *this;
    }

    void operator+=(TData const& value)
    {
        offsetValue(value);
    }

    [[nodiscard]]
    float getTime() const override
    {
        if (!m_use_realtime) {
            return App::getTime();
        } else {
            return App::getTimeWall();
        }
    }

    [[nodiscard]]
    float getInterpolationRatio() const
    {
        return std::min(1.0f, getValueRatio());
    }

private:
    TData m_start_value;
    TData m_target_value;
    TData m_delta;

    bool m_use_realtime{false};
};

using InterpolatedFloat = InterpolatedData<float>;
using InterpolatedVec2  = InterpolatedData<Vec2f>;
using InterpolatedVec3  = InterpolatedData<Vec3f>;
using InterpolatedVec4  = InterpolatedData<Vec4f>;


/// Specialization for Colors
template<>
struct InterpolatedData<sf::Color> final : public Interpolable
{
public:
    void setValue(sf::Color const color)
    {
        m_data.setValue(pez::getVec4<Vec4f>(color));
    }

    void setValueDirect(sf::Color const color)
    {
        m_data.setValueDirect(pez::getVec4<Vec4f>(color));
    }

    void operator=(sf::Color const color)
    {
        setValue(color);
    }

    /// Casts the value to the underlying type by returning the current value
    operator sf::Color() const
    {
        return getCurrentValue();
    }

    [[nodiscard]]
    sf::Color getCurrentValue() const
    {
        return getColor(m_data.getCurrentValue());
    }

    [[nodiscard]]
    bool isDone() const override
    {
        return m_data.isDone();
    }

    [[nodiscard]]
    float getTime() const override
    {
        return m_data.getTime();
    }

    void useRealtime()
    {
        m_data.useRealtime();
    }

    /// Sets the function used for interpolation
    void setInterpolationFunction(InterpolationFunction const function) override
    {
        m_data.setInterpolationFunction(function);
    }

    /// Sets the interpolation speed
    void setInterpolationSpeed(float const speed) override
    {
        m_data.setInterpolationSpeed(speed);
    }

    /// Sets interpolation function and speed
    void setInterpolation(InterpolationFunction const function, float const speed) override
    {
        m_data.setInterpolation(function, speed);
    }

private:
    InterpolatedData<Vec4f> m_data;
};

}