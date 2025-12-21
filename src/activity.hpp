#pragma once
#include <charconv>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>


struct Activity final
{
    std::string name;
    sf::Color   color;

    Activity() = default;

    Activity(std::string const& name_, sf::Color const color_)
        : name{name_}
        , color{color_}
    {

    }

    static Activity fromString(std::string const& str)
    {
        Activity result;

        std::istringstream stream(str);

        auto const readNum = [&]() -> std::optional<uint8_t> {
            std::string word;
            stream >> word;

            int32_t value;
            auto const conv_result = std::from_chars(word.data(), word.data() + word.size(), value);
            if (conv_result.ec == std::errc()) {
                return value;
            }
            return std::nullopt;
        };

        // Parse the line
        stream >> result.name;
        auto const red = readNum();
        auto const green = readNum();
        auto const blue = readNum();

        if (!red || !green || !blue) {
            result.color = sf::Color::White;
        } else {
            result.color = {*red, *green, *blue};
        }

        return result;
    }
};
