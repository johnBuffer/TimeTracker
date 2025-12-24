#pragma once
#include <vector>

#include "peztool/utils/binary_io.hpp"

#include "./date.hpp"

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
            addEntry(getMidnight(), 0);
        }
    }

    ~History()
    {
        saveToFile(getCurrentSaveFile());
    }

    /// Adds a new activity entry in the history
    void addEntry(Date const& date, size_t const activity_idx)
    {
        // It's useless to add multiple times the same activity
        if (!entries.empty() && entries.back().activity_idx == activity_idx) {
            return;
        }
        entries.emplace_back(date, activity_idx);
    }

    /// Returns the total duration of the provided activity
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

    /// Given an activity returns its proportion in the day
    [[nodiscard]]
    float getDurationPercent(size_t const activity_idx) const
    {
        float const current_seconds = Date::now().getTimeAsSeconds();
        return (getDuration(activity_idx) / current_seconds) * 100.0f;
    }

    /// Returns the index of the last activity
    [[nodiscard]]
    size_t getLastActivityIdx() const
    {
        return entries.back().activity_idx;
    }

    /// Starts a new day that continues the last one
    void newDay(Date const& last_day)
    {
        // Save the last day
        saveToFile(getSaveFile(last_day));
        // Use last day's ongoing activity as today's first one
        size_t const first_activity_idx{getLastActivityIdx()};
        entries.clear();
        addEntry(getMidnight(), first_activity_idx);
    }

    /// Saves the current history to a file
    void saveToFile(std::string const& filename) const
    {
        size_t const entry_count = entries.size();
        size_t save_from_idx = 0;
        if (std::filesystem::exists(filename)) {
            std::cout << "Save found for today, appending" << std::endl;
            auto const last_data = load(filename).back();
            for (; save_from_idx < entry_count; ++save_from_idx) {
                if (entries[save_from_idx].date.getTimeAsSeconds() > last_data.date.getTimeAsSeconds()) {
                    break;
                }
            }
        } else {
            std::cout << "No file found, creating a new one" << std::endl;
        }

        std::cout << "Saving from idx " << save_from_idx << std::endl;
        std::ofstream file(filename, std::ios::app);
        for (size_t i{save_from_idx}; i < entry_count; ++i) {
            file << entries[i].toString() << '\n';
        }
        file.close();
    }

    /// Loads a list of entries from the provided @p filename
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

    /// Creates an Entry for a string
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
        result.date = Date{data[0], data[1], data[2], data[3], data[4], data[5], 0};
        result.activity_idx = data[6];
        return result;
    }

    /// Returns the save filename for today
    [[nodiscard]]
    static std::string getCurrentSaveFile()
    {
        return getSaveFile(Date::now());
    }

    /// Returns the save filename for today
    [[nodiscard]]
    static std::string getSaveFile(Date const& date)
    {
        return std::format("data/history/{}{:0>2}{:0>2}.txt", date.year, date.month, date.day);
    }

private:
    [[nodiscard]]
    static Date getMidnight()
    {
        Date midnight = Date::now();
        midnight.setTime(0, 0, 0);
        return midnight;
    }
};