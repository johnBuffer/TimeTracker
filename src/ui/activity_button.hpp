#pragma once
#include "./history.hpp"
#include "./ui_common.hpp"
#include "peztool/utils/color_utils.hpp"
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "standard/widget.hpp"
#include "utils.hpp"


struct ActivityBackground : public sf::Transformable, public sf::Drawable
{
    static float constexpr text_scale_f = 0.3f;
    static Vec2f constexpr text_scale   = {text_scale_f, text_scale_f};

    std::string activity_label;

    sf::Font const& font_title;
    sf::Font const& font_timer;
    pez::CardOutlined background;

    float duration = 0.0f;
    float percent = 0.0f;

    explicit
    ActivityBackground(pez::ResourcesStore const& store, Vec2f const size_)
        : font_title{*store.getFont("font_medium")}
        , font_timer{*store.getFont("font_mono")}
        , background{ui::createBackground(size_)}
    {

    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(background, states);

        drawTitle(target, states);
        drawDuration(target, states);
    }

    void setShadowOffset(Vec2f const offset)
    {
        background.background.shadow_offset = offset;
    }

    void setOutlineThickness(float const thickness, bool const skip_update = false)
    {
        background.setOutlineThickness(thickness, skip_update);
    }

    void setSize(Vec2f const size, bool const skip_update = false)
    {
        background.setOuterSize(size, skip_update);
    }

    void setFillColor(sf::Color const color)
    {
        background.setFillColor(color);
    }

    [[nodiscard]]
    Vec2f getSize() const
    {
        return background.getOutlineSize();
    }

    void drawTitle(sf::RenderTarget& target, sf::RenderStates const& states) const
    {
        sf::Text title{font_title, activity_label, 200};
        title.setScale(text_scale);
        title.setFillColor(pez::setAlpha(sf::Color::White, 200));
        {
            auto const bounds = title.getLocalBounds();
            title.setOrigin(bounds.position + Vec2f{bounds.size.x * 0.5f, 0.0f});
        }
        title.setPosition({getSize().x * 0.5f, ui::margin});
        target.draw(title, states);

        sf::Text percent_label{font_timer, std::format("{:.0f}%", percent), 100};
        percent_label.setScale(text_scale);
        percent_label.setFillColor(pez::setAlpha(sf::Color::White, 200));
        {
            auto const bounds = percent_label.getLocalBounds();
            percent_label.setOrigin(bounds.position + Vec2f{bounds.size.x * 0.5f, 0.0f});
            percent_label.setPosition({getSize().x * 0.5f, getSize().y - ui::margin - bounds.size.y * text_scale_f});
        }
        target.draw(percent_label, states);
    }

    void drawDuration(sf::RenderTarget& target, sf::RenderStates const& states) const
    {
        sf::Text text{font_timer, "00:00:00", 150};
        text.setScale(text_scale);
        text.setFillColor(pez::setAlpha(sf::Color::White, 150));
        auto const bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + Vec2f{bounds.size.x * 0.5f, 0.0f});
        text.setString(timeToString(duration));

        Vec2f const size = getSize();
        text.setPosition(size * 0.5f);
        target.draw(text, states);
    }
};


struct ActivityButton final : ui::Widget
{
    using Ptr = std::shared_ptr<ActivityButton>;

    enum class State
    {
        Active,
        Idle,
    };

    // State
    State  state = State::Idle;
    size_t activity_idx;

    // Render info
    sf::Font const& font;

    History const* history;

    pez::InterpolatedFloat highlight_offset;
    pez::InterpolatedFloat highlight_scale;
    pez::InterpolatedFloat outline;

    pez::InterpolatedFloat background_height;

    std::function<void()> on_activate;

    ActivityBackground background;

    pez::CardOutlined background_base;

    pez::InterpolatedFloat margin;

    explicit
    ActivityButton(pez::ResourcesStore const& store, Vec2f const size_, size_t const activity_idx_, History const& history_)
        : ui::Widget{size_}
        , activity_idx{activity_idx_}
        , font{*store.getFont("font_medium")}
        , history{&history_}
        , background{store, size_}
        , background_base{ui::createBackground(size_)}
    {
        margin.setValueDirect(0.0f);

        Vec2f const background_size = background.getSize();
        background.setOrigin(background_size * 0.5f);
        background.setPosition(background_size);

        auto constexpr interpolation_function = pez::InterpolationFunction::EaseInOutQuint;

        highlight_offset.setInterpolationSpeed(5.0f);
        highlight_offset.setInterpolationFunction(interpolation_function);
        highlight_offset.setValueDirect(0.0f);
        highlight_offset.useRealtime();

        highlight_scale.setInterpolationSpeed(5.0f);
        highlight_scale.setInterpolationFunction(interpolation_function);
        highlight_scale.setValueDirect(1.0f);
        highlight_scale.useRealtime();

        background_height.setInterpolationSpeed(2.0f);
        background_height.setInterpolationFunction(interpolation_function);
        background_height.setValueDirect(size->y);
        background_height.useRealtime();

        outline.setInterpolationSpeed(2.0f);
        outline.setInterpolationFunction(interpolation_function);
        outline.setValueDirect(0.0f);
        outline.useRealtime();
    }

    void onUpdate(float const dt) override
    {
        if (!background_height.isDone()) {
            float const height = background_height;
            background.setOutlineThickness(outline, true);
            background.setSize({size->x, height});
        }
        float const offset = highlight_offset;
        Vec2f const background_size = *size;
        background.setPosition(background_size * 0.5f + Vec2f{0.0f, offset});
        background.setShadowOffset({0.0f, 10.0f - offset});
        float const scale = highlight_scale;
        background.setScale({scale, scale});

        background.duration = history->getDuration(activity_idx);
        background.percent = (background.duration / Date::now().getTimeAsSeconds()) * 100.0f;

        if (pez::App::getTimeWall() > 1.0f) {
            margin = 40.0f;
        }

        float const m = margin;
        background_base.setOuterSize(*size - Vec2f{m * 1.2f, 3.0f * m});
        background_base.setPosition(Vec2f{m - (m * 0.2f * activity_idx), 2.0f * m});
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        /*float constexpr led_radius = 10.0f;
        float constexpr led_offset = 40.0f;

        sf::Text text{font, "Active", 192};
        text.setScale(ActivityBackground::text_scale);
        auto const bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + bounds.size * 0.5f);

        float const text_offset = size->y - background.getSize().y;
        text.setPosition({size->x * 0.5f + led_offset * 0.25f, size->y * 0.7f + text_offset});
        text.setFillColor(pez::setAlpha(sf::Color::White, 200));
        target.draw(text, states);

        pez::Card led{2.0f * Vec2f{led_radius, led_radius}, led_radius, sf::Color::Green};
        led.shadow_color = sf::Color::Green;
        led.setShadowSize(16.0f);
        led.setPosition(text.getPosition() - Vec2f{bounds.size.x * ActivityBackground::text_scale_f * 0.5f + led_offset, 8.0f});
        target.draw(led, states);

        target.draw(background, states);*/
        target.draw(background_base, states);
    }

    bool onClick(Vec2f const) override
    {
        activate();
        return true;
    }

    void onMouseEnter() override
    {
        if (state == State::Idle) {
            //highlight();
        }
    }

    void onMouseExit() override
    {
        //resetHighlight();
    }

    void activate()
    {
        state = State::Active;
        background_height = size->y * 0.8f;
        outline = 10.0f;
        resetHighlight();

        if (on_activate) {
            on_activate();
        }
    }

    void deactivate()
    {
        state = State::Idle;
        background_height = size->y;
        outline = 0.0f;
    }

private:
    void highlight()
    {
        highlight_offset = -ui::margin * 0.25f;
        highlight_scale = 1.02f;
    }

    void resetHighlight()
    {
        highlight_offset = 0.0f;
        highlight_scale = 1.0f;
    }
};
