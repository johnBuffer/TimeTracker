#pragma once
#include <functional>
#include "./async_task.hpp"

namespace pez
{
struct AsyncTaskLambda final : AsyncTask
{
public:
    template<typename TCallback>
    void setCallback(TCallback&& callback)
    {
        m_callback = std::forward<TCallback>(callback);
    }

    void task() override
    {
        if (m_callback) {
            m_callback();
        }
    }

    void trigger()
    {
        runTask();
    }

private:
    std::function<void()> m_callback;
};
}