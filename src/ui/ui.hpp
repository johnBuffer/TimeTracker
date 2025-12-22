#pragma once

#include "peztool/core/system.hpp"
#include "peztool/utils/render/blur/blur.hpp"
#include "peztool/utils/render/utils.hpp"

#include "standard/drawer.hpp"
#include "standard/text_label.hpp"

#include "configuration.hpp"

#include "./activity_button.hpp"
#include "./activity_container.hpp"
#include "./day_overview_bar.hpp"
#include "./time_bar.hpp"
#include "./slot_info.hpp"

using EntitiesUI = pez::RequiredEntity<>;
using ProcessorsUI = pez::RequiredSystems<>;
using RenderersUI = pez::RequiredSystems<>;
using RendererUI = pez::Renderer<EntitiesUI, ProcessorsUI, RenderersUI>;


/// The UI renderer drawing all the user interface elements
struct UI final : RendererUI
{
    static float constexpr time_bar_height = 100.0f;

    History& history = pez::Singleton<History>::get();
    Configuration const& configuration = pez::Singleton<Configuration>::get();

    sf::Font const& font;

    ui::Widget::Ptr root{nullptr};

    TextLabel::Ptr timer_label;
    TextLabel::Ptr day_overview_label;
    TimeBar::Ptr time_bar_global;
    DayOverviewBar::Ptr day_overview_bar;

    ActivityContainer::Ptr activity_container;
    size_t current_activity{0};
    SlotInfo slot_info;

    Blur background_blur;

    UI(Vec2f const render_size_, pez::ResourcesStore const& store_)
        : RendererUI{render_size_, store_}
        , font{getFontMedium()}
        , slot_info{font, configuration.activities}
        , background_blur{Vec2u{render_size_}}
    {
        // Create the root widget, parent of all widgets
        root = std::make_shared<ui::Widget>(m_render_size);
        // Initialize blur shaders
        CardShader::get().setRenderSize(m_render_size);
        background_blur.kernel.setRadius(4.5f);
    }

    void onInitialized() override
    {
        initializeUI();
    }

    void render(pez::RenderContext& context) override
    {
        root->update(pez::App::getDt());
        context.draw(*root);

        sf::RenderStates states;
        if (slot_info.scale > 0.0f) {
            auto const& blur_texture = background_blur.apply(context);
            states.texture = &blur_texture;
        }

        if (day_overview_bar->slot_hover) {
            std::cout << "LOL" << std::endl;
            auto const& hover = *day_overview_bar->slot_hover;
            float const day_height = day_overview_bar->size->y;
            Vec2f const slot_position = day_overview_bar->getPosition() + Vec2f{hover.slot_position_x, day_height - ui::element_spacing};
            Vec2f const info_position = slot_position + Vec2f{0.0f, 2.0f * ui::element_spacing};
            slot_info.setHover(hover, info_position);
        } else {
            slot_info.setVisible(false);
        }
        slot_info.update();
        context.draw(slot_info, states);
    }

    void initializeUI()
    {
        int32_t constexpr   label_size  = 32;
        float constexpr     label_x     = ui::margin + 0.5f * ui::background_radius;
        sf::Color constexpr label_color = pez::setAlpha(sf::Color::White, 200);

        size_t const activity_count = configuration.activities.size();
        Vec2f const time_bar_size{m_render_size.x - 2.0f * ui::margin, time_bar_height};

        float current_y = ui::margin;
        Vec2f const time_bar_size{m_render_size.x - 2.0f * ui::margin, time_bar_height};

        day_overview_label = root->createChild<TextLabel>(font);
        day_overview_label->setString("Timeline");
        day_overview_label->setPosition({label_x, current_y});
        day_overview_label->setCharacterSize(label_size);
        day_overview_label->setFillColor(label_color);
        current_y += 0.35f * ui::margin + day_overview_label->size->y;

        day_overview_bar = root->createChild<DayOverviewBar>(time_bar_size, history, configuration.activities);
        day_overview_bar->setPosition({ui::margin, current_y});
        current_y += ui::margin + time_bar_height;

        day_overview_label = root->createChild<TextLabel>(font);
        day_overview_label->setString("Timeline");
        day_overview_label->setPosition({label_x, current_y});
        day_overview_label->setCharacterSize(label_size);
        day_overview_label->setFillColor(label_color);
        current_y += 0.35f * ui::margin + day_overview_label->size->y;

        day_overview_bar = root->createChild<DayOverviewBar>(time_bar_size, history, configuration.activities);
        day_overview_bar->setPosition({ui::margin, current_y});
        current_y += 0.75f * ui::margin + time_bar_height;

        timer_label = root->createChild<TextLabel>(font);
        timer_label->setString("Distribution");
        timer_label->setPosition({label_x, current_y});
        timer_label->setCharacterSize(label_size);
        timer_label->setFillColor(label_color);
        current_y += 0.35f * ui::margin + timer_label->size->y;

        time_bar_global = root->createChild<TimeBar>(font, time_bar_size, history, configuration.activities);
        time_bar_global->setPosition({ui::margin, current_y});
        current_y += 1.5f * ui::margin + time_bar_height;

        float const activity_margin = ui::margin;
        Vec2f const activity_container_size = {
            m_render_size.x - 2.0f * activity_margin,
            m_render_size.y - activity_margin - current_y
        };
        activity_container = root->createChild<ActivityContainer>(activity_container_size);
        activity_container->setPosition({activity_margin, current_y});

        auto const activity_count_f = static_cast<float>(activity_count);
        float const activity_height = activity_container_size.y - 2.0f * activity_margin;
        float const activity_width = (activity_container_size.x - ui::margin * (activity_count_f + 1.0f)) / activity_count_f;
        Vec2f const activity_size = {activity_width, activity_height};
        for (size_t i = 0; i < activity_count; ++i) {
            Activity const& activity = configuration.activities[i];
            auto const activity_button = activity_container->createChild<ActivityButton>(m_resources, activity_size, i, history);
            float const y = ui::margin;
            float const x = ui::margin + static_cast<float>(i) * (activity_width + ui::margin);
            activity_button->setPosition({x, y});
            activity_button->on_activate = [this, i] {
                activate(i);
            };
            activity_button->background.setFillColor(configuration.activities[i].color);
            activity_button->background.activity_label = configuration.activities[i].name;
        }

        size_t const last_activity = history.getLastActivityIdx();
        auto const& button = activity_container->getButton(last_activity);
        if (button) {
            //button->activate();
        } else {
            std::cout << std::format("Could not activate activity [{}]", last_activity);
        }
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

        activity_container->getButton(current_activity)->deactivate();
        current_activity = activity_idx;
        activity_container->getButton(current_activity)->activate();
        history.addEntry(Date::now(), current_activity);
    }
};
