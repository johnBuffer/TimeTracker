#pragma once
#include <filesystem>
#include <string_view>

inline bool createIfDoesntExist(std::filesystem::path const& path)
{
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
        return false;
    }
    return true;
}

inline void checkDataDirectory()
{
    std::filesystem::path const data_dir_name = "data";
    std::filesystem::path const history_dir_name = data_dir_name / "history";
    if (!createIfDoesntExist(data_dir_name)) {
        // We know that history does not exist
        std::filesystem::create_directories(history_dir_name);
    } else {
        // History might not be there for some reason
        createIfDoesntExist(history_dir_name);
    }
    // The correct directories should be created at this point
}

inline std::string timeToString(float const seconds)
{
    int32_t const minutes = static_cast<int32_t>(seconds) / 60;
    int32_t const hours   = minutes / 60;
    return std::format(
        "{:0>2}:{:0>2}:{:0>2}",
        hours,
        minutes % 60,
        static_cast<int32_t>(seconds) % 60);
}
