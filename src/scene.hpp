#pragma once
#include "peztool/core/scene.hpp"
#include "peztool/utils/watcher.hpp"

#include "ui/ui.hpp"


using Entities = pez::EntityPack<>;
using Processors = pez::SystemPack<>;
using Renderers = pez::SystemPack<UI>;


struct TimeTracker final : pez::Scene<Entities, Processors, Renderers>
{
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
        handler.onKeyPressed(sf::Keyboard::Key::Escape, [&](sf::Event::KeyPressed) {
            pez::App::exit();
        });

        std::function on_focus = [](sf::Event::FocusLost const&) {
        };
        handler.addCallback(on_focus);

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
        m_render_context->setClearColor({80, 80, 80});
    }

    void onTick(float dt) override
    {

    }
};
