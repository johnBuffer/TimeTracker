#pragma once
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./day_overview_bar.hpp"


struct ActivityInfo final : sf::Transformable, sf::Drawable
{
    static Vec2f constexpr s_size{300.0f, 200.0f};

    std::vector<Activity> const* activities;

    sf::Font const& font;
    pez::Card background;

    pez::InterpolatedVec2 position;
    pez::InterpolatedFloat scale;
    bool visible = false;

    DayOverviewBar::SlotHover current_hover;

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

        float const margin{2.0f * ui::element_spacing};

        sf::Text text{font, (*activities)[current_hover.activity_idx].name, 32};
        ui::setOrigin(text, ui::origin::Mode::TopCenter);
        text.setPosition({s_size.x * 0.5f, margin});
        target.draw(text, states);

        text.setCharacterSize(48);
        text.setString("00:00:00");
        auto const bounds = text.getLocalBounds();
    }

    void setHover(DayOverviewBar::SlotHover const& hover, Vec2f const position_)
    {
        if (hover.slot_position_x != current_hover.slot_position_x) {
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
            current_hover.slot_position_x = 0.0f;
            scale = 0.0f;
        }
    }
};
