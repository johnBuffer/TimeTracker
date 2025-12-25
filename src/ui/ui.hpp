#pragma once

#include "configuration.hpp"
#include "peztool/core/system.hpp"
#include "peztool/utils/render/blur/blur.hpp"
#include "peztool/utils/render/utils.hpp"
#include "standard/drawer.hpp"
#include "standard/text_label.hpp"

#include "./ui_data.hpp"
#include "./activity_button.hpp"
#include "./activity_info.hpp"
#include "./container.hpp"
#include "./day_overview_bar.hpp"
#include "./slot_info.hpp"
#include "./time_bar.hpp"

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

    ui::Widget::Ptr root;
    TextLabel::Ptr time_label;
    TimeBar::Ptr time_bar_global;
    DayOverviewBar::Ptr day_overview_bar;

    std::vector<ActivityButton::Ptr> buttons;
    size_t current_activity{0};

    SlotInfo slot_info;
    ActivityInfo activity_info;

    Blur background_blur;

    UI(Vec2f const render_size_, pez::ResourcesStore const& store_)
        : RendererUI{render_size_, store_}
        , font{getFontMedium()}
        , slot_info{font, configuration.activities}
        , activity_info{font, configuration.activities}
        , background_blur{Vec2u{render_size_}}
    {
        pez::Singleton<ui::Data>::create();

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
        time_label->setString(timeToString(Date::now().getTimeAsSeconds()));
        context.draw(*root);

        sf::RenderStates states;

        if (slot_info.scale > 0.0f || activity_info.scale > 0.0f) {
            auto const& blur_texture = background_blur.apply(context);
            states.texture = &blur_texture;
        }

        if (day_overview_bar->slot_hover) {
            auto const& hover = *(day_overview_bar->slot_hover);
            float const day_height = day_overview_bar->size->y;
            Vec2f const slot_position = day_overview_bar->getPosition() + Vec2f{hover.x, day_height - ui::element_spacing};
            Vec2f const info_position = slot_position + Vec2f{0.0f, 2.0f * ui::element_spacing};
            slot_info.setHover(hover, info_position);
        } else {
            slot_info.setVisible(false);
        }
        slot_info.update();
        context.draw(slot_info, states);

        if (time_bar_global->activity_hover) {
            auto const& hover = *(time_bar_global->activity_hover);
            float const day_height = time_bar_global->size->y;
            Vec2f const slot_position = time_bar_global->getPosition() + Vec2f{hover.x, day_height - ui::element_spacing};
            Vec2f const info_position = slot_position + Vec2f{0.0f, 2.0f * ui::element_spacing};
            activity_info.setHover(hover, info_position);
        } else {
            activity_info.setVisible(false);
        }
        activity_info.update();
        context.draw(activity_info, states);
    }

    void initializeUI()
    {
        ui::Data& data = pez::Singleton<ui::Data>::get();
        data.scale = m_render_size.y / 1080.0f;

        float const margin = data.getScaled(ui::margin);

        float const time_bar_height_scaled = data.getScaled(time_bar_height);
        size_t const activity_count = configuration.activities.size();
        Vec2f const time_bar_size{m_render_size.x - 2.0f * margin, time_bar_height_scaled};

        float current_y = margin;

        time_label = root->createChild<TextLabel>(getFontMedium());
        time_label->setPosition({m_render_size.x * 0.5f, current_y});
        time_label->setString("00:00:00");
        time_label->setCharacterSize(120);
        time_label->setOrigin({time_label->size->x * 0.5f, 0.0f});
        time_label->setFillColor(pez::setAlpha(sf::Color::White, 200));
        time_label->setScale(data.getScaleVec());
        current_y += 1.5f * margin + time_label->getEffectiveSize().y;

        day_overview_bar = root->createChild<DayOverviewBar>(time_bar_size, history, configuration.activities);
        day_overview_bar->setPosition({margin, current_y});
        current_y += 1.0f * margin + time_bar_height_scaled;

        time_bar_global = root->createChild<TimeBar>(font, time_bar_size, history, configuration.activities);
        time_bar_global->setPosition({margin, current_y});
        current_y += 1.5f * margin + time_bar_height_scaled;

        Vec2f const activity_container_size = {
            m_render_size.x - 2.0f * margin,
            m_render_size.y - margin - current_y
        };
        auto const activity_container = root->createChild<Container>(activity_container_size);
        activity_container->background.setRadius(ui::background_radius + margin);
        activity_container->setPosition({margin, current_y});

        auto const activity_count_f = static_cast<float>(activity_count);
        float const activity_height = activity_container_size.y - 2.0f * margin;
        float const activity_width = (activity_container_size.x - margin * (activity_count_f + 1.0f)) / activity_count_f;
        Vec2f const activity_size = {activity_width, activity_height};
        for (size_t i = 0; i < activity_count; ++i) {
            auto const activity_button = activity_container->createChild<ActivityButton>(m_resources, activity_size, i, history);
            float const x = margin + static_cast<float>(i) * (activity_width + margin);
            activity_button->setPosition({x, margin});
            activity_button->on_activate = [this, i] {
                activate(i);
            };
            activity_button->background.setFillColor(configuration.activities[i].color);
            activity_button->background.activity_label = configuration.activities[i].name;
            buttons.push_back(activity_button);
        }

        size_t const last_activity = history.getLastActivityIdx();
        if (last_activity < buttons.size()) {
            buttons[last_activity]->activate();
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

        buttons[current_activity]->deactivate();
        current_activity = activity_idx;
        buttons[current_activity]->activate();
        history.addEntry(Date::now(), current_activity);
    }
};
