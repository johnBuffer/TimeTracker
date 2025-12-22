#pragma once
#include "./ui_common.hpp"
#include "./history.hpp"
#include "standard/widget.hpp"


struct DayOverviewBar final : ui::Widget
{
    struct SlotHover
    {
        size_t activity_idx{};
        float  slot_position_x{};
        float  start_time{};
        float  end_time{};
    };

    using Ptr = std::shared_ptr<DayOverviewBar>;

    // Data
    History const* history{};
    std::vector<Activity> const* activities;

    // Render
    pez::CardOutlined background;
    sf::RenderTexture chart_texture;

    // Hover
    std::optional<SlotHover> slot_hover;

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
        //target.draw(chart, states);
    }

    bool onClick(Vec2f const) override
    {
        return true;
    }

    void onMouseMove(Vec2f const pos) override
    {
        slot_hover = getSlotHover(pos.x);
    }

    void onMouseExit() override
    {
        slot_hover = std::nullopt;
    }

private:
    [[nodiscard]]
    Vec2f getAvailableSize() const
    {
        float constexpr total_space = 2.0f * ui::element_spacing;
        return *size - Vec2f{total_space, total_space};
    }

    [[nodiscard]]
    std::optional<SlotHover> getSlotHover(float const x) const
    {
        size_t const entry_count = history->entries.size();
        Vec2f const available_size = getAvailableSize();
        float constexpr day_seconds = 3600.0f * 24.0f;

        auto const getSlotRangeX = [&](float const start_time, float const end_time) {
            float const  x_start   = ui::element_spacing + available_size.x * (start_time / day_seconds);
            float const  x_end     = ui::element_spacing + available_size.x * (end_time / day_seconds);
            return Vec2f{x_start, x_end};
        };

        auto const& entries = history->entries;
        // Check all entries except the last
        for (size_t i = 0; i < entry_count - 1; ++i) {
            Vec2f const start_end = {entries[i].date.getTimeAsSeconds(), entries[i + 1].date.getTimeAsSeconds()};
            Vec2f const range     = getSlotRangeX(start_end.x, start_end.y);
            if (x > range.x && x < range.y) {
                return SlotHover{entries[i].activity_idx, x, start_end.x, start_end.y};
            }
        }
        // Check the last (ongoing) one
        Vec2f const start_end = {entries.back().date.getTimeAsSeconds(), Date::now().getTimeAsSeconds()};
        Vec2f const last_range = getSlotRangeX(start_end.x, start_end.y);
        if (x > last_range.x && x < last_range.y) {
            return SlotHover{entries[entry_count - 1].activity_idx, x, start_end.x, start_end.y};
        }

        return std::nullopt;
    }
};
