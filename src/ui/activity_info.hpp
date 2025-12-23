#pragma once
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./time_bar.hpp"


struct ActivityInfo final : sf::Transformable, sf::Drawable
{
    static Vec2f constexpr s_size{300.0f, 160.0f};

    std::vector<Activity> const* activities;

    sf::Font const& font;
    pez::Card background;

    pez::InterpolatedVec2 position;
    pez::InterpolatedFloat scale;
    bool visible = false;

    TimeBar::ActivityHover current_hover;

    explicit
    ActivityInfo(sf::Font const& font_, std::vector<Activity> const& activities_)
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

        float constexpr margin{2.0f * ui::element_spacing};

        Activity const& current_activity = (*activities)[current_hover.activity_idx];
        sf::Text text{font, current_activity.name, ui::info_box_title_size};
        ui::setOrigin(text, ui::origin::Mode::TopCenter);
        text.setPosition({s_size.x * 0.5f, margin});
        target.draw(text, states);

        text.setCharacterSize(ui::info_box_value_size);
        text.setFillColor(current_activity.color);
        text.setString(std::format("{:.0f}%", current_hover.ratio * 100.0f));
        {
            auto const bounds = text.getLocalBounds();
            text.setOrigin(bounds.position + bounds.size * 0.5f);
            text.setPosition(s_size * 0.5f);
            target.draw(text, states);
        }

        text.setCharacterSize(ui::info_box_small_size);
        text.setFillColor(pez::setAlpha(sf::Color::White, 150));
        text.setString(timeToString(current_hover.duration));
        {
            auto const bounds = text.getLocalBounds();
            text.setOrigin(bounds.position + Vec2f{bounds.size.x * 0.5f, bounds.size.y});
            float const slot_times_y = s_size.y - margin;
            text.setPosition({s_size.x * 0.5f, slot_times_y});
            target.draw(text, states);
        }
    }

    void setHover(TimeBar::ActivityHover const& hover, Vec2f const position_)
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
