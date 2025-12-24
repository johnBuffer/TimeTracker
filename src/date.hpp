#pragma once
#include <cstdint>
#include <chrono>

struct Date
{
    int32_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t minute;
    int32_t second;
    int32_t millisecond;

    Date() = default;

    Date(int32_t const year_, int32_t const month_, int32_t const day_, int32_t const hour_, int32_t const minute_, int32_t const second_, int32_t const millisecond_)
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
        auto const now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        // Platform-specific thread-safe conversion
        std::tm local_tm;
#ifdef _WIN32
        localtime_s(&local_tm, &now_c);
#else
        localtime_r(&now_c, &local_tm);
#endif
        int32_t const year = local_tm.tm_year + 1900;
        int32_t const month = local_tm.tm_mon + 1;
        int32_t const day = local_tm.tm_mday;
        int32_t const hour = local_tm.tm_hour;
        int32_t const minute = local_tm.tm_min;
        int32_t const second = local_tm.tm_sec;
        int32_t const ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        return {year, month, day, hour, minute, second, ms};
    }
};
