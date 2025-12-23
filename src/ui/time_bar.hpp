#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct TimeBar final : ui::Widget
{
    struct ActivityInfo
    {
        size_t activity_idx{};
        float  duration{};
        float  width{};
        float  ratio{};
        float  x{};
    };

    struct ActivityHover
    {
        size_t activity_idx{};
        float  duration{};
        float  ratio{};
    };

    using Ptr = std::shared_ptr<TimeBar>;

    History const* history;
    std::vector<Activity> const* activities;

    std::vector<ActivityInfo> info;

    // Hover
    std::optional<ActivityHover> activity_hover;

    sf::Font const& font;
    pez::CardOutlined background;

    explicit
    TimeBar(sf::Font const& font_, Vec2f const size_, History const& history_, std::vector<Activity> const& activities_)
        : ui::Widget{size_}
        , history{&history_}
        , activities{&activities_}
        , info(activities->size())
        , font{font_}
        , background{ui::createBackground(size_)}
    {
    }

    void onUpdate(float const dt) override
    {
        size_t const activity_count = activities->size();

        float total_time = 0.0f;
        size_t slot_count = 0;

        for (size_t i = 1; i < activity_count; ++i) {
            info[i].activity_idx = i;
            info[i].duration = history->getDuration(i);
            total_time += info[i].duration;
            slot_count += (info[i].duration > 0.0f);
        }

        float const total_width = size->x - 2.0f * ui::element_spacing - static_cast<float>(slot_count - 1) * ui::element_spacing;
        float current_x = ui::element_spacing;
        for (size_t i = 1; i < activity_count; ++i) {
            info[i].ratio = info[i].duration / total_time;
            info[i].width = info[i].ratio * total_width;
            info[i].x = current_x;
            current_x += info[i].width + ui::element_spacing;
        }
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);

        float const height = size->y - 2.0f * ui::element_spacing;
        for (auto const& a : info) {
            Activity const& activity = (*activities)[a.activity_idx];
            if (a.duration == 0.0f) {
                continue;
            }
            pez::Card time_slot{{a.width, height}, ui::background_radius - ui::element_spacing, activity.color};
            time_slot.setPosition({a.x, ui::element_spacing});
            time_slot.shadow_offset = {0.0f, 2.0f};
            target.draw(time_slot, states);
        }
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }

    void onMouseMove(Vec2f const pos) override
    {
        checkActivity(pos.x);
    }

    void checkActivity(float const x)
    {
        size_t const activity_count = info.size();
        for (size_t i{0}; i < activity_count; ++i) {
            auto const& a = info[i];
            if (x > a.x && x < a.x + a.width) {
                activity_hover = {a.activity_idx, a.duration, a.ratio};
                return;
            }
        }
        activity_hover = std::nullopt;
    }
};
