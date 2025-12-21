#pragma once
#include <vector>
#include <string>
#include "./activity.hpp"

struct Configuration
{
    std::vector<Activity> activities;

    Configuration()
    {
        loadFromFile("data/conf.txt");
    }

    void loadFromFile(std::string const& filename)
    {
        activities.clear();
        activities.emplace_back("Other", sf::Color{120, 120, 120});

        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                activities.push_back(Activity::fromString(line));
            }
            file.close();
        } else {
            std::cout << "No configuration file found." << std::endl;
        }
    }
};