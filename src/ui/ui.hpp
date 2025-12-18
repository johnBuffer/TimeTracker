#pragma once
#include "peztool/core/system.hpp"
#include "peztool/utils/render/blur/blur.hpp"

#include "standard/drawer.hpp"

#include "./activity_button.hpp"
#include "./time_bar.hpp"


using EntitiesUI = pez::RequiredEntity<>;
using ProcessorsUI = pez::RequiredSystems<>;
using RenderersUI = pez::RequiredSystems<>;
using RendererUI = pez::Renderer<EntitiesUI, ProcessorsUI, RenderersUI>;


/// The UI renderer drawing all the user interface elements
struct UI final : RendererUI
{
    static float constexpr time_bar_height = 100.0f;

    sf::Font const& font;

    ui::Widget::Ptr root{nullptr};

    TimeBar::Ptr time_bar_global;
    TimeBar::Ptr time_bar_relative;

    std::vector<ActivityButton::Ptr> activities;
    std::vector<float> activities_time;
    size_t current_activity{0};
    float start_time{0.0f};
    sf::Clock clock;

    Blur background_blur;

    UI(Vec2f const render_size_, pez::ResourcesStore const& store_)
        : RendererUI{render_size_, store_}
        , font{getFontMedium()}
        , background_blur{Vec2u{render_size_}}
    {
        // Create the root widget, parent of all widgets
        root = std::make_shared<ui::Widget>(m_render_size);

        CardShader::get().setRenderSize(m_render_size);
        background_blur.kernel.setRadius(2.5f);
    }

    void onInitialized() override
    {
        initializeUI();
    }

    void updateTime()
    {
        activities_time[current_activity] += clock.getElapsedTime().asSeconds();
        clock.restart();
    }

    void render(pez::RenderContext& context) override
    {
        updateTime();
        auto const& blur_texture = background_blur.apply(context);

        sf::RenderStates states;
        states.texture = &blur_texture;

        root->update(pez::App::getDt());
        time_bar_global->times = activities_time;
        for (size_t i = 0; i < activities.size(); ++i) {
            activities[i]->background.duration = activities_time[i];
        }
        context.draw(*root, states);
    }

    void initializeUI()
    {
        size_t const activity_count = 4;

        float current_y = ui::margin;
        Vec2f const time_bar_size{m_render_size.x - 2.0f * ui::margin, time_bar_height};
        time_bar_global = root->createChild<TimeBar>(font, time_bar_size, activity_count);
        time_bar_global->setPosition({ui::margin, current_y});
        current_y += ui::margin + time_bar_height;

        time_bar_relative = root->createChild<TimeBar>(font, time_bar_size, activity_count);
        time_bar_relative->setPosition({ui::margin, current_y});
        current_y += ui::margin + time_bar_height;

        std::array palette{
            sf::Color{239, 71, 111},
            sf::Color{255, 209, 102},
            sf::Color{6, 214, 160},
            sf::Color{17, 138, 178}
        };

        std::array labels{
            "Activity 0",
            "Activity 1",
            "Activity 2",
            "Activity 3",
        };

        auto const activity_count_f = static_cast<float>(activity_count);
        float const activity_height = m_render_size.y - current_y - ui::margin;
        float const activity_width = (m_render_size.x - ui::margin * (activity_count_f + 1.0f)) / activity_count_f;
        Vec2f const activity_size = {activity_width, activity_height};
        for (size_t i = 0; i < activity_count; ++i) {
            auto const activity = root->createChild<ActivityButton>(m_resources, activity_size);
            float const y = current_y;
            float const x = ui::margin + static_cast<float>(i) * (activity_width + ui::margin);
            activity->setPosition({x, y});
            activity->on_activate = [this, i]() {
                activate(i);
            };
            activity->background.setFillColor(palette[i]);
            activity->background.activity_label = labels[i];
            activities.push_back(activity);
        }

        activities_time.resize(activity_count, 0.0f);
        activities[0]->activate();
    }

    void scaleWidgets(Vec2f const scale) const
    {
        for (auto const& w : root->children) {
            w->setScale(scale);
        }
    }

    [[nodiscard]]
    sf::Font const& getFontRegular() const
    {
        return *getFont("font_regular");
    }

    [[nodiscard]]
    sf::Font const& getFontMedium() const
    {
        return *getFont("font_medium");
    }

    void onMouseMove(Vec2f const mouse_position) const
    {
        root->mouseMove(mouse_position);
    }

    void activate(size_t const activity_idx)
    {
        if (activity_idx == current_activity) {
            return;
        }

        activities[current_activity]->deactivate();
        current_activity = activity_idx;
        activities[current_activity]->activate();
    }
};
