#pragma once
#include <vector>
#include <string>


struct Configuration
{
    std::vector<std::string> activities;

    Configuration()
    {
        loadFromFile("data/conf.txt");
    }

    void loadFromFile(std::string const& filename)
    {
        activities.clear();

        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                activities.push_back(line);
            }
            file.close();
        } else {
            std::cerr << "Unable to open file" << std::endl;
        }
    }
};