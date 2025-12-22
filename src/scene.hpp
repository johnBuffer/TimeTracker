#pragma once
#include "peztool/core/scene.hpp"

#include "ui/ui.hpp"
#include "./history.hpp"


using Entities = pez::EntityPack<>;
using Processors = pez::SystemPack<>;
using Renderers = pez::SystemPack<UI>;


struct TimeTracker final : pez::Scene<Entities, Processors, Renderers>
{
    sf::Clock new_day_check_delay;
    Date launch_date = Date::now();

    TimeTracker()
    {
        // Fonts
        m_resources.registerFont("res/roboto_regular.ttf", "font_regular");
        m_resources.registerFont("res/roboto_medium.ttf", "font_medium");
        m_resources.registerFont("res/roboto_mono_semi.ttf", "font_mono");
    }

    /// Creates the scene's events
    void registerEvents(pez::EventHandler& handler) override
    {
        std::function on_closed = [](sf::Event::Closed const&) {
            pez::App::exit();
        };
        handler.addCallback(on_closed);

        std::function on_focus_lost = [](sf::Event::FocusLost const&) {
            pez::App::setFramerateLimit(20);
        };
        handler.addCallback(on_focus_lost);

        std::function on_focus = [](sf::Event::FocusGained const&) {
            pez::App::enableVSync();
        };
        handler.addCallback(on_focus);

        handler.onKeyPressed(sf::Keyboard::Key::Escape, [&](sf::Event::KeyPressed) {
            pez::App::exit();
        });

        handler.onMouseMoved([&](sf::Event::MouseMoved const&)
        {
            auto const& renderer = getRenderer<UI>();
            renderer.onMouseMove(getMousePosition());
        });

        handler.onMousePressed(sf::Mouse::Button::Left, [&](sf::Event::MouseButtonPressed) {
            auto const& renderer = getRenderer<UI>();
            if (!renderer.root->click(getMousePosition())) {
                m_render_context->mouse_clicked = true;
            }
        });

        handler.onMouseReleased(sf::Mouse::Button::Left, [&](sf::Event::MouseButtonReleased) {
            if (m_render_context->mouse_clicked) {
                m_render_context->mouse_clicked = false;
            }
            auto const& renderer = getRenderer<UI>();
            renderer.root->unclick(getMousePosition());
        });

        handler.onMouseReleased(sf::Mouse::Button::Right, [&](sf::Event::MouseButtonReleased) {
        });
    }

    /// This is executed after all systems have been loaded
    void onInitialized() override
    {
        pez::App::enableVSync();
        m_render_context->setClearColor({80, 80, 80});

        getRenderer<UI>().window_header->title = "TimeTracker";
    }

    void onTick(float dt) override
    {
        checkWindowMove();

        if (new_day_check_delay.getElapsedTime().asSeconds() > 2.0f) {
            new_day_check_delay.restart();
            checkNewDay();
        }

        if (!isMouseInWindow()) {
            auto const& renderer = getRenderer<UI>();
            if (!renderer.root->isClicked()) {
                renderer.root->mouseExit();
            }
        }
    }

    [[nodiscard]]
    bool isMouseInWindow() const
    {
        auto const mouse_position = static_cast<Vec2f>(sf::Mouse::getPosition(m_render_context->getWindow()));
        Vec2f const window_size = m_render_context->getRenderSize();
        sf::FloatRect const bounding_box{{}, window_size};
        return bounding_box.contains(mouse_position);
    }

    void checkNewDay()
    {
        Date const now = Date::now();
        if (now.day != launch_date.day) {
            pez::Singleton<History>::get().newDay(launch_date);
            launch_date = now;
        }
    }

    void checkWindowMove()
    {
        auto& window = m_render_context->getWindow();
        auto& ui = getRenderer<UI>();
        auto& window_header = ui.window_header;
        if (window_header->isClicked()) {
            Vec2i const mouse_pos = sf::Mouse::getPosition();
            Vec2i const delta = mouse_pos - window_header->click_position;
            window_header->click_position = mouse_pos;
            Vec2i const new_position = window.getPosition() + delta;
            window.setPosition(new_position);
        }
    }
};
