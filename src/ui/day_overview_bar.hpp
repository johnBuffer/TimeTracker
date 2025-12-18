#pragma once
#include "./ui_common.hpp"
#include "./history.hpp"
#include "standard/widget.hpp"


struct DayOverviewBar final : ui::Widget
{
    using Ptr = std::shared_ptr<DayOverviewBar>;

    static float constexpr s_margin = 10.0f;

    pez::CardOutlined background;

    History const* history{};

    sf::RenderTexture chart_texture;

    explicit DayOverviewBar(sf::Font const& font_, Vec2f const size_, History const& history_)
        : ui::Widget{size_}
        , background{ui::createBackground(size_)}
        , history{&history_}
    {
        float const width  = size->x - 2.0f * s_margin;
        float const height = size->y - 2.0f * s_margin;
        if (!chart_texture.resize({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})) {
            std::cout << "Unable to create chart texture" << std::endl;
        }
    }

    void onUpdate(float const dt) override
    {
        std::array constexpr palette{
            sf::Color{120, 120, 120},
            sf::Color{239, 71, 111},
            sf::Color{255, 209, 102},
            sf::Color{6, 214, 160},
            sf::Color{17, 138, 178}
        };

        size_t const entry_count = history->entries.size();
        pez::QuadVertexArray vertex_array;

        float const width  = size->x - 2.0f * s_margin;
        float const height = size->y - 2.0f * s_margin;
        float const day_seconds = 3600.0f * 24.0f;

        //chart_texture.clear(background.background.color);
        chart_texture.clear({80, 80, 80});
        auto const createSlot = [&](float const start_time, float const end_time, sf::Color const color) {
            float const  x_start   = width * (start_time / day_seconds);
            float const  x_end     = width * (end_time / day_seconds);
            Vec2f const  slot_size = {x_end - x_start, height};
            size_t const idx       = vertex_array.appendAlignedRectangle(slot_size, Vec2f{x_start, 0.0f} + slot_size * 0.5f);
            vertex_array.setQuadColor(idx, color);
        };

        auto const& entries = history->entries;
        for (size_t i = 0; i < entry_count - 1; ++i) {
            createSlot(entries[i].date.getTimeAsSeconds(), entries[i + 1].date.getTimeAsSeconds(), palette[entries[i].activity_idx]);
        }
        createSlot(entries.back().date.getTimeAsSeconds(), Date::now().getTimeAsSeconds(), palette[entries.back().activity_idx]);
        chart_texture.draw(vertex_array);
        chart_texture.display();
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.texture = nullptr;
        target.draw(background, states);

        float const width  = size->x - 2.0f * s_margin;
        float const height = size->y - 2.0f * s_margin;
        pez::Card chart{{width, height}, ui::background_radius - s_margin, sf::Color::White};
        chart.setTexture(&chart_texture.getTexture());
        chart.shadow_offset = {0.0f, 2.0f};
        chart.setPosition({s_margin, s_margin});
        target.draw(chart, states);
    }

    bool onClick(Vec2f const) override { return true; }
};
