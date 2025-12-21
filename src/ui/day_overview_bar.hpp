#pragma once
#include "./ui_common.hpp"
#include "./history.hpp"
#include "standard/widget.hpp"


struct DayOverviewBar final : ui::Widget
{
    using Ptr = std::shared_ptr<DayOverviewBar>;

    // Data
    History const* history{};
    std::vector<Activity> const* activities;

    // Render
    pez::CardOutlined background;
    sf::RenderTexture chart_texture;

    explicit DayOverviewBar(Vec2f const size_, History const& history_, std::vector<Activity> const& activities_)
        : ui::Widget{size_}
        , history{&history_}
        , activities{&activities_}
        , background{ui::createBackground(size_)}
    {
        auto const texture_size = static_cast<Vec2u>(getAvailableSize());
        if (!chart_texture.resize(texture_size)) {
            std::cout << "Unable to create chart texture" << std::endl;
        }
    }

    void onUpdate(float const dt) override
    {
        size_t const entry_count = history->entries.size();
        pez::QuadVertexArray vertex_array;

        Vec2f const available_size = getAvailableSize();
        float constexpr day_seconds = 3600.0f * 24.0f;

        chart_texture.clear({50, 50, 50});
        auto const createSlot = [&](float const start_time, float const end_time, sf::Color const color) {
            float const  x_start   = available_size.x * (start_time / day_seconds);
            float const  x_end     = available_size.x * (end_time / day_seconds);
            Vec2f const  slot_size = {x_end - x_start, available_size.y};
            size_t const idx       = vertex_array.appendAlignedRectangle(slot_size, Vec2f{x_start, 0.0f} + slot_size * 0.5f);
            vertex_array.setQuadColor(idx, color);
        };

        auto const& entries = history->entries;
        auto const getSlotColor = [&](size_t const slot_idx) -> sf::Color {
            size_t const activity_idx = entries[slot_idx].activity_idx;
            return (*activities)[activity_idx].color;
        };

        for (size_t i = 0; i < entry_count - 1; ++i) {
            createSlot(entries[i].date.getTimeAsSeconds(), entries[i + 1].date.getTimeAsSeconds(), getSlotColor(i));
        }
        createSlot(entries.back().date.getTimeAsSeconds(), Date::now().getTimeAsSeconds(), getSlotColor(entry_count - 1));
        chart_texture.draw(vertex_array);
        chart_texture.display();
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.texture = nullptr;
        target.draw(background, states);

        Vec2f const card_size = getAvailableSize();
        pez::Card chart{card_size, ui::background_radius - ui::element_spacing, sf::Color::White};
        chart.setTexture(&chart_texture.getTexture());
        chart.shadow_offset = {0.0f, 2.0f};
        chart.setPosition({ui::element_spacing, ui::element_spacing});
        target.draw(chart, states);
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }

private:
    [[nodiscard]]
    Vec2f getAvailableSize() const
    {
        float constexpr total_space = 2.0f * ui::element_spacing;
        return *size - Vec2f{total_space, total_space};
    }
};
