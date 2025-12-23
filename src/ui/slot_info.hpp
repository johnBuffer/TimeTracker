#pragma once
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./day_overview_bar.hpp"


struct SlotInfo final : sf::Transformable, sf::Drawable
{
    static Vec2f constexpr s_size{300.0f, 160.0f};

    std::vector<Activity> const* activities;

    sf::Font const& font;
    pez::Card background;

    pez::InterpolatedVec2 position;
    pez::InterpolatedFloat scale;
    bool visible = false;

    DayOverviewBar::SlotHover current_hover;

    explicit
    SlotInfo(sf::Font const& font_, std::vector<Activity> const& activities_)
        : activities{&activities_}
        , font{font_}
        , background{s_size, ui::background_radius, {200, 200, 200}}
    {
        setOrigin({s_size.x * 0.5f, 0.0f});
        background.setShadowSize(40.0f);
        background.shadow_offset = {0.0f, 10.0f};
        background.blur_background = true;

        position.setInterpolationSpeed(8.0f);
        scale.setInterpolationSpeed(4.0f);
    }

    void update()
    {
        setPosition(position);
        float const s{scale};
        setScale({s, s});
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(background, states);

        float const margin{2.0f * ui::element_spacing};

        Activity const& current_activity = (*activities)[current_hover.activity_idx];

        sf::Text text{font, current_activity.name, ui::info_box_title_size};
        ui::setOrigin(text, ui::origin::Mode::TopCenter);
        text.setPosition({s_size.x * 0.5f, margin});
        target.draw(text, states);

        text.setCharacterSize(ui::info_box_value_size);
        text.setFillColor(current_hover.activity_idx > 0 ? current_activity.color : sf::Color{220, 220, 220});
        {
            text.setString("00:00:00");
            auto const bounds = text.getLocalBounds();
            text.setString(timeToString(current_hover.end_time - current_hover.start_time));
            text.setOrigin(bounds.position + bounds.size * 0.5f);
        }
        text.setPosition(s_size * 0.5f);
        target.draw(text, states);
        text.setCharacterSize(ui::info_box_small_size);
        text.setFillColor(pez::setAlpha(sf::Color::White, 150));

        {
            text.setString("00:00:00");
            auto const bounds = text.getLocalBounds();
            float const slot_times_y = s_size.y - margin;
            text.setPosition({margin, slot_times_y});

            text.setString(timeToString(current_hover.start_time));
            text.setOrigin(bounds.position + Vec2f{0.0f, bounds.size.y});
            target.draw(text, states);

            text.setString(timeToString(current_hover.end_time));
            text.setPosition({s_size.x - margin, slot_times_y});
            text.setOrigin(bounds.position + Vec2f{bounds.size.x, bounds.size.y});
            target.draw(text, states);
        }

    }

    void setHover(DayOverviewBar::SlotHover const& hover, Vec2f const position_)
    {
        if (hover.x != current_hover.x) {
            position.setValueDirect(position_);
            current_hover = hover;
            setVisible(true);
        }
    }

    void setVisible(bool const v)
    {
        if (v != visible) {
            visible = v;
        }
        if (visible) {
            scale = 1.0f;
        } else {
            current_hover.x = 0.0f;
            scale = 0.0f;
        }
    }
};
