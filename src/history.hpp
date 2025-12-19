#pragma once
#include <chrono>
#include <vector>

#include "peztool/utils/binary_io.hpp"


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

struct History
{
    struct TimePoint
    {
        Date   date{};
        size_t activity_idx{0};

        [[nodiscard]]
        bool isSame(TimePoint const& other) const
        {
            return (activity_idx == other.activity_idx) && date.isSame(other.date);
        }

        [[nodiscard]]
        std::string toString() const
        {
            return std::format("{} {} {} {} {} {} {}",
                date.year,
                date.month,
                date.day,
                date.hour,
                date.minute,
                date.second,
                activity_idx
            );
        }
    };

    std::vector<TimePoint> entries;

    History()
    {
        entries = load(getCurrentSaveFile());
        // No entry, create the midnight default entry
        if (entries.empty()) {
            Date midnight = Date::now();
            midnight.setTime(0, 0, 0);
            addEntry(midnight, 0);
        }
    }

    ~History()
    {
        saveToFile();
    }

    void addEntry(Date const& date, size_t const activity_idx)
    {
        // It's useless to add multiple times the same activity
        if (!entries.empty() && entries.back().activity_idx == activity_idx) {
            return;
        }
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

    [[nodiscard]]
    float getDurationPercent(size_t const activity_idx) const
    {
        float const current_seconds = Date::now().getTimeAsSeconds();
        return (getDuration(activity_idx) / current_seconds) * 100.0f;
    }

    void saveToFile() const
    {
        std::string const current_file = getCurrentSaveFile();

        size_t const entry_count = entries.size();
        size_t save_from_idx = 0;
        if (std::filesystem::exists(current_file)) {
            std::cout << "Save found for today, appending" << std::endl;
            auto const last_data = load(current_file).back();
            for (; save_from_idx < entry_count; ++save_from_idx) {
                if (entries[save_from_idx].date.getTimeAsSeconds() > last_data.date.getTimeAsSeconds()) {
                    break;
                }
            }
        } else {
            std::cout << "No file found, creating a new one" << std::endl;
        }

        std::cout << "Saving from idx " << save_from_idx << std::endl;
        std::ofstream file(current_file, std::ios::app);
        for (size_t i{save_from_idx}; i < entry_count; ++i) {
            file << entries[i].toString() << '\n';
        }
        file.close();
    }

    [[nodiscard]]
    static std::vector<TimePoint> load(std::string const& filename)
    {
        std::vector<TimePoint> data;

        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                auto const time_point = loadFromString(line);
                if (time_point) {
                    data.push_back(*time_point);
                }
            }
            file.close();
        } else {
            std::cout << "No save file found for today, a new one will be created." << std::endl;
        }
        return data;
    }

    [[nodiscard]]
    static std::optional<TimePoint> loadFromString(std::string const& line)
    {
        std::stringstream stream(line);

        int32_t payload;
        std::vector<int32_t> data;

        // Extract all integers
        while (stream >> payload) {
            data.push_back(payload);
        }

        if (data.size() != 7) {
            std::cout << "Incorrect string, aborting" << std::endl;
            return std::nullopt;
        }

        TimePoint result;
        result.date = Date{data[0], static_cast<uint32_t>(data[1]), static_cast<uint32_t>(data[2]), data[3], data[4], data[5], 0};
        result.activity_idx = data[6];
        return result;
    }

    [[nodiscard]]
    static std::string getCurrentSaveFile()
    {
        auto const now = Date::now();
        return std::format("{}{:0>2}{:0>2}.txt", now.year, now.month, now.day);
    }
};