#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct TimeBar final : ui::Widget
{
    using Ptr = std::shared_ptr<TimeBar>;

    History const* history;
    size_t activity_count;

    sf::Font const& font;

    pez::CardOutlined background;

    explicit
    TimeBar(sf::Font const& font_, Vec2f const size_, History const& history_, size_t const activity_count_)
        : ui::Widget{size_}
        , history{&history_}
        , activity_count{activity_count_}
        , font{font_}
        , background{ui::createBackground(size_)}
    {
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);

        std::vector<float> durations(activity_count, 0.0f);

        size_t slot_count = 0;
        float total_time = 0.0f;
        for (size_t i = 1; i < activity_count; ++i) {
            durations[i] = history->getDuration(i);
            total_time += durations[i];
            slot_count += durations[i] > 0.0f;
        }

        if (slot_count == 0) {
            return;
        }

        std::array const palette{
            sf::Color{120, 120, 120},
            sf::Color{239, 71, 111},
            sf::Color{255, 209, 102},
            sf::Color{6, 214, 160},
            sf::Color{17, 138, 178}
        };

        float const margin = 10.0f;
        float const height = size->y - 2.0f * margin;
        float const total_width = size->x - 2.0f * margin - static_cast<float>(slot_count - 1) * margin;
        float current_x = margin;
        for (size_t i = 1; i < activity_count; ++i) {
            if (durations[i] == 0.0f) {
                continue;
            }
            float const width = (durations[i] / total_time) * total_width;
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
