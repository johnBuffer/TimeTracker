#pragma once
#include <filesystem>

namespace pez
{

/// Sets the working directory as the directory containing the executable
inline void setWorkingDirectoryToExecutablePath(char* executable_path)
{
    auto const containing_folder = std::filesystem::path{executable_path}.parent_path();
    std::filesystem::current_path(containing_folder);
}
}