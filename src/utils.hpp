#pragma once
#include <filesystem>
#include <string_view>


inline void checkDataDirectory()
{
    std::string_view constexpr data_dir_name = "data";
    if (!std::filesystem::exists(data_dir_name)) {
        if (std::filesystem::create_directories(data_dir_name)) {
            std::cout << "Directory created.\n";
        } else {
            std::cout << "Failed to create directory.\n";
        }
    } else {
        std::cout << "Directory already exists.\n";
    }
}
