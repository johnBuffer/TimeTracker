#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct TimeBar final : ui::Widget
{
    using Ptr = std::shared_ptr<TimeBar>;

    History const* history;
    std::vector<Activity> const* activities;

    sf::Font const& font;
    pez::CardOutlined background;

    explicit
    TimeBar(sf::Font const& font_, Vec2f const size_, History const& history_, std::vector<Activity> const& activities_)
        : ui::Widget{size_}
        , history{&history_}
        , activities{&activities_}
        , font{font_}
        , background{ui::createBackground(size_)}
    {
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        if (pez::App::getTimeWall() > 3.0f) {
            target.draw(background, states);
        }

        size_t const activity_count = activities->size();
        std::vector durations(activity_count, 0.0f);

        size_t slot_count = 0;
        float total_time = 0.0f;
        for (size_t i = 1; i < activity_count; ++i) {
            durations[i] = history->getDuration(i);
            total_time += durations[i];
            slot_count += durations[i] > 0.0f;
        }

        float const height = size->y - 2.0f * ui::element_spacing;
        float const total_width = size->x - 2.0f * ui::element_spacing - static_cast<float>(slot_count - 1) * ui::element_spacing;
        float current_x = ui::element_spacing;
        for (size_t i = 1; i < activity_count; ++i) {
            Activity const& activity = (*activities)[i];
            if (durations[i] == 0.0f) {
                continue;
            }
            float const width = (durations[i] / total_time) * total_width;
            pez::Card time_slot{{width, height}, ui::background_radius - ui::element_spacing, activity.color};
            time_slot.setPosition({current_x, ui::element_spacing});
            time_slot.shadow_offset = {0.0f, 2.0f};
            //target.draw(time_slot, states);
            current_x += width + ui::element_spacing;
        }
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }
};
