#pragma once
#include <cstdint>
#include <chrono>

struct Date
{
    int32_t year;
    uint32_t month;
    uint32_t day;

    int32_t hour;
    int32_t minute;
    int32_t second;
    int32_t millisecond;

    Date() = default;

    Date(int32_t const year_, uint32_t const month_, uint32_t const day_, int32_t const hour_, int32_t const minute_, int32_t const second_, int32_t const millisecond_)
        : year{year_}
        , month{month_}
        , day{day_}
        , hour{hour_}
        , minute{minute_}
        , second{second_}
        , millisecond{millisecond_}
    {

    }

    void setTime(int32_t const hour_, int32_t const minute_, int32_t const second_)
    {
        hour = hour_;
        minute = minute_;
        second = second_;
    }

    [[nodiscard]]
    float getTimeAsSeconds() const
    {
        return static_cast<float>(hour * 3600 + minute * 60 + second) + static_cast<float>(millisecond) / 1000.0f;
    }

    [[nodiscard]]
    bool isSame(Date const& other) const
    {
        return (year == other.year) && (month == other.month) && (day == other.day) && (hour == other.hour) && (minute == other.minute) && (second == other.second);
    }

    static Date now()
    {
        auto const now = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()};
        auto const local_time_point = now.get_local_time();

        // Split into date and time
        auto const date = std::chrono::floor<std::chrono::days>(local_time_point);
        std::chrono::year_month_day ymd{date};

        // Extract all components
        auto const year = static_cast<int32_t>(ymd.year());
        auto const month = static_cast<uint32_t>(ymd.month());
        auto const day = static_cast<uint32_t>(ymd.day());

        std::chrono::hh_mm_ss const time{local_time_point - date};
        auto const hour = time.hours().count();
        auto const minute = time.minutes().count();
        auto const second = static_cast<int32_t>(time.seconds().count());
        auto const millisecond = static_cast<int32_t>(duration_cast<std::chrono::milliseconds>(time.subseconds()).count());
        return {year, month, day, hour, minute, second, millisecond};
    }
};
