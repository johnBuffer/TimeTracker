#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct TimeBar final : ui::Widget
{
    using Ptr = std::shared_ptr<TimeBar>;

    sf::Font const& font;

    pez::CardOutlined background;

    std::vector<float> times;

    explicit
    TimeBar(sf::Font const& font_, Vec2f const size_, size_t const activity_count)
        : ui::Widget{size_}
        , font{font_}
        , background{ui::createBackground(size_)}
    {
        times.resize(activity_count);
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);

        size_t slot_count = 0;
        float total_time = 0.0f;
        for (auto const t : times) {
            total_time += t;
            slot_count += t > 0.0f;
        }

        if (slot_count == 0) {
            return;
        }

        std::array<sf::Color, 4> const palette{
            sf::Color{239, 71, 111},
            sf::Color{255, 209, 102},
            sf::Color{6, 214, 160},
            sf::Color{17, 138, 178}
        };

        float const margin = 10.0f;
        float const height = size->y - 2.0f * margin;
        float const total_width = size->x - 2.0f * margin - static_cast<float>(slot_count - 1) * margin;
        float current_x = margin;
        for (size_t i = 0; i < times.size(); ++i) {
            if (times[i] == 0.0f) {
                continue;
            }
            float const width = (times[i] / total_time) * total_width;
            pez::Card time_slot{{width, height}, ui::background_radius - margin, palette[i]};
            time_slot.setPosition({current_x, margin});
            target.draw(time_slot, states);
            current_x += width + margin;
        }
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }
};
