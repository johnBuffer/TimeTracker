#pragma once
#include <functional>
#include "peztool/peztool.hpp"


namespace pez
{

struct Cooldown
{
    explicit
    Cooldown(float const period, bool const use_wall_time = false)
        : m_period{period}
        , m_use_wall_time{use_wall_time}
        , m_start_time{getCurrentTime()}
    {
    }

    void setCallback(std::function<void(float)>&& callback)
    {
        m_callback = std::move(callback);
    }

    void update()
    {
        float const current_time = getCurrentTime();
        float const elapsed_time = current_time - m_start_time;
        if (elapsed_time >= m_period) {
            float const delta = elapsed_time - m_period;
            m_start_time = current_time - delta;
            if (m_callback) {
                m_callback(delta);
            }
        }
    }

    [[nodiscard]]
    float getPeriod() const
    {
        return m_period;
    }

private:
    float m_period;
    bool m_use_wall_time;
    float m_start_time;
    std::function<void(float)> m_callback;

    [[nodiscard]]
    float getCurrentTime() const
    {
        return m_use_wall_time ? App::getTimeWall() : App::getTime();
    }
};

}

