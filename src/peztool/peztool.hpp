#pragma once
#include <SFML/Graphics.hpp>

#include "peztool/core/scene.hpp"
#include "peztool/core/static_interface.hpp"
#include "peztool/utils/thread_pool.hpp"
#include "peztool/utils/configuration_loader.hpp"


namespace pez
{
/** This class is responsible for storing entities, renderers, and processors
 *
 */
class App
{
public:
    App(std::string const& title, std::string const& conf_filename)
    {
        loadConfiguration(conf_filename);
        m_window.setTitle(title);
        setTickRate(120, true);
        setMouseCursorVisible(true);

        // Create default singletons
        GlobalInstance<App>::instance = this;
    }

    void setTickRate(uint32_t const tick_rate, bool const sync_window_frame_limit)
    {
        m_tick_rate = tick_rate;
        if (sync_window_frame_limit) {
            setWindowFrameRateLimit(tick_rate);
        }
        m_dt = 1.0f / static_cast<float>(m_tick_rate);
    }

    void setWindowFrameRateLimit(uint32_t const frame_rate_limit)
    {
        m_window.setFramerateLimit(frame_rate_limit);
    }

    void setMouseCursorVisible(bool const b)
    {
        m_window.setMouseCursorVisible(b);
    }

    /// Processes events and checks if the apps must exit
    void run()
    {
        while (m_window.isOpen()) {
            tick(m_dt);
        }
    }

    /// Closes the window and stops the application
    void close()
    {
        m_window.close();
    }

    void tick(float const dt)
    {
        if (m_current_scene) {
            m_current_scene->setRunning(m_running);
            m_current_scene->tick(dt);
        }
        if (m_running) {
            // Update time
            m_time += dt;
            ++m_tick;
        }
    }

    void toggleMaxFramerateInternal()
    {
        if (m_frame_rate_unlocked) {
            setWindowFrameRateLimit(m_tick_rate);
        } else {
            setWindowFrameRateLimit(0);
        }

        m_frame_rate_unlocked = !m_frame_rate_unlocked;
    }

    template<typename TScene, typename... TArgs>
    TScene& addScene(TArgs&&... args)
    {
        static_assert(std::is_base_of_v<SceneBase, TScene>);
        m_current_scene = std::make_unique<TScene>(std::forward<TArgs>(args)...);;
        m_current_scene->initialize(m_window, m_render_size);
        return dynamic_cast<TScene&>(*m_current_scene);
    }

    template<typename TScene>
    TScene& getScene()
    {
        return dynamic_cast<TScene&>(*m_current_scene);
    }

    static float getTime()
    {
        return GlobalInstance<App>::instance->m_time;
    }

    static size_t getTick()
    {
        return GlobalInstance<App>::instance->m_tick;
    }

    static float getTimeWall()
    {
        return GlobalInstance<App>::instance->m_time_wall.getElapsedTime().asSeconds();
    }

    static void exit()
    {
        GlobalInstance<App>::instance->close();
    }

    static sf::Vector2u getRenderSize()
    {
        return GlobalInstance<App>::instance->m_render_size;
    }

    static void toggleMaxFramerate()
    {
        GlobalInstance<App>::instance->toggleMaxFramerateInternal();
    }

    static void setFramerateLimit(uint32_t const frame_rate_limit)
    {
        GlobalInstance<App>::instance->m_window.setFramerateLimit(frame_rate_limit);
        GlobalInstance<App>::instance->m_frame_rate_unlocked = false;
    }

    [[nodiscard]]
    static bool isFramerateUnlocked()
    {
        return GlobalInstance<App>::instance->m_frame_rate_unlocked;
    }

    static ThreadPool& getThreadPool()
    {
        return Singleton<ThreadPool>::get();
    }

    static void togglePause()
    {
        GlobalInstance<App>::instance->m_running = !(GlobalInstance<App>::instance->m_running);
    }

    static bool isRunning()
    {
        return GlobalInstance<App>::instance->m_running;
    }

    static uint32_t getTickRate()
    {
        return GlobalInstance<App>::instance->m_tick_rate;
    }

    static float getDt()
    {
        return 1.0f / static_cast<float>(getTickRate());
    }

private:
    sf::RenderWindow m_window;
    sf::Vector2u m_render_size;

    uint32_t  m_tick_rate;
    float     m_dt;
    float     m_time = 0.0f;
    size_t    m_tick = 0;
    sf::Clock m_time_wall;

    bool m_running = true;
    bool m_frame_rate_unlocked = false;

    std::unique_ptr<SceneBase> m_current_scene = nullptr;

    /// Reads configuration file and sets the app accordingly
    void loadConfiguration(std::string const& conf_filename)
    {
        // Load user configuration
        cload::ConfigurationLoader const loader{conf_filename};
        // Load window size
        Vec2u window_size{1920, 1080};
        loader.tryReadSequenceIntoArray<2>("window_size", &window_size.x);
        m_render_size = window_size;
        // Load state (fullscreen or windowed)
        auto const fullscreen = loader.tryGetValueAs<bool>("fullscreen").value_or(false);
        sf::State const state{fullscreen ? sf::State::Fullscreen : sf::State::Windowed};
        // Set AA level
        sf::ContextSettings settings{};
        settings.antiAliasingLevel = 8;
        // Create the window
        m_window.create(sf::VideoMode{{window_size.x, window_size.y}}, "", sf::Style::Default, state, settings);
    }
};

}  // namespace pez
