#pragma once
#include <chrono>
#include <vector>


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

struct History
{
    struct TimePoint
    {
        Date   date{};
        size_t activity_idx{0};
    };

    std::vector<TimePoint> entries;

    History()
    {
        Date midnight = Date::now();
        midnight.setTime(0, 0, 0);
        addEntry(midnight, 0);
    }

    void addEntry(Date const& date, size_t const activity_idx)
    {
        entries.emplace_back(date, activity_idx);
    }

    [[nodiscard]]
    float getDuration(size_t const activity_idx) const
    {
        auto const getSlotDuration = [](Date const& start, Date const& end) {
            return end.getTimeAsSeconds() - start.getTimeAsSeconds();
        };
        float result = 0.0f;
        size_t const activity_count = entries.size();
        for (size_t i = 0; i < activity_count - 1; ++i) {
            if (entries[i].activity_idx == activity_idx) {
                result += getSlotDuration(entries[i].date, entries[i + 1].date);
            }
        }
        if (entries.back().activity_idx == activity_idx) {
            result += getSlotDuration(entries.back().date, Date::now());
        }
        return result;
    }
};