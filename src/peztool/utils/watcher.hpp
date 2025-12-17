#pragma once

namespace pez
{
/// Very naive object watcher, to check that value has changed
template<typename TType>
struct Watcher
{
private:
    TType const* m_watched{nullptr};
    TType m_last{};
    bool  m_first_time{true};

public:
    Watcher() = default;

    explicit
    Watcher(TType const& w)
        : m_watched{&w}
    , m_last{w}
    {}

    bool checkChange()
    {
        if (m_watched && ((*m_watched != m_last) || m_first_time)) {
            m_first_time = false;
            m_last = *m_watched;
            return true;
        }
        return false;
    }
};
}