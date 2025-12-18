#pragma once
#include "./ui_common.hpp"
#include "./history.hpp"
#include "standard/widget.hpp"


struct DayOverviewBar final : ui::Widget
{
    using Ptr = std::shared_ptr<DayOverviewBar>;

    pez::CardOutlined background;

    History const* history{};

    explicit DayOverviewBar(sf::Font const& font_, Vec2f const size_, History const& history_)
        : ui::Widget{size_}
        , background{ui::createBackground(size_)}
        , history{&history_}
    {
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.texture = nullptr;
        target.draw(background, states);

        size_t const entry_count = history->entries.size();
        pez::QuadVertexArray vertex_array;

        float const margin = ui::background_radius;
        float const width  = size->x - 2.0f * margin;
        float const height = size->y - 2.0f * margin;
        float const day_seconds = 3600.0f * 24.0f;

        std::array palette{
            sf::Color{120, 120, 120},
            sf::Color{239, 71, 111},
            sf::Color{255, 209, 102},
            sf::Color{6, 214, 160},
            sf::Color{17, 138, 178}
        };

        auto const createSlot = [&](float const start_time, float const end_time, sf::Color const color) {
            float const  x_start   = width * (start_time / day_seconds);
            float const  x_end     = width * (end_time / day_seconds);
            Vec2f const  slot_size = {x_end - x_start, height};
            size_t const idx       = vertex_array.appendAlignedRectangle(slot_size, Vec2f{margin + x_start, margin} + slot_size * 0.5f);
            vertex_array.setQuadColor(idx, color);
        };

        auto const& entries = history->entries;
        for (size_t i = 0; i < entry_count - 1; ++i) {
            createSlot(entries[i].date.getTimeAsSeconds(), entries[i + 1].date.getTimeAsSeconds(), palette[entries[i].activity_idx]);
        }
        createSlot(entries.back().date.getTimeAsSeconds(), Date::now().getTimeAsSeconds(), palette[entries.back().activity_idx]);

        target.draw(vertex_array, states);
    }

    bool onClick(Vec2f const) override { return true; }
};
