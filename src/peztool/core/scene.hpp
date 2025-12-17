#pragma once
#include "../utils/events.hpp"
#include "../utils/resources.hpp"
#include "./container.hpp"
#include "./render.hpp"

namespace pez
{

class SceneBase
{
public:
    virtual ~SceneBase() = default;

    void initialize(sf::RenderWindow& window, sf::Vector2u render_size)
    {
        m_event_handler = std::make_unique<EventHandler>(window);
        registerDefaultEvents();
        registerEvents(*m_event_handler);
        m_render_context = std::make_unique<RenderContext>(window, render_size);
        m_render_context->createDefaultLayers(*m_event_handler);
        onInitializedInternal();
    }

    virtual void onInitializedInternal() = 0;

    virtual void registerEvents(EventHandler& handler) = 0;

    void tick(float const dt)
    {
        m_render_context->clear();
        onTickInternal(*m_render_context, dt);
        m_render_context->renderLayers();
        m_event_handler->processEvents();
    }

    virtual void onTickInternal(RenderContext& context, float dt) = 0;

    void setZoom(float const zoom)
    {
        m_render_context->getWorldLayer().setZoom(zoom);
    }

    void setCameraPosition(Vec2f position)
    {
        m_render_context->getWorldLayer().setViewPosition(position);
    }

    [[nodiscard]]
    Vec2f getMousePosition() const
    {
        return m_render_context->getMousePosition();
    }

    [[nodiscard]]
    Vec2f getMouseWorldPosition() const
    {
        return m_render_context->getMouseWorldPosition();
    }

    void setRunning(bool const running)
    {
        m_running = running;
    }

protected:
    [[nodiscard]]
    bool isRunning() const
    {
        return m_running;
    }

    void toggleRender()
    {
        m_skip_render = !m_skip_render;
    }

    std::unique_ptr<EventHandler>  m_event_handler;
    std::unique_ptr<RenderContext> m_render_context;
    ResourcesStore m_resources;

    bool m_skip_render = false;

private:
    bool m_running = true;

private:
    void registerDefaultEvents() const
    {
        m_event_handler->addCallback<sf::Event::Resized>([this](sf::Event::Resized const)
        {
            m_render_context->updateMousePositionScale();
        });
    }
};


/// This class stores all the assets declared by the user
template<typename TEntitySet, typename TProcessorSet, typename TRendererSet>
class Scene : public SceneBase
{
public:
    Scene() = default;
    ~Scene() override = default;

    [[nodiscard]]
    size_t getExecutionTimeUs() const
    {
        return m_execution_time_us;
    }

    [[nodiscard]]
    float getExecutionTimeMs() const
    {
        return static_cast<float>(m_execution_time_us) * 0.001f;
    }

private:
    friend class App;

    TEntitySet    m_entities;
    TProcessorSet m_processors;
    TRendererSet  m_renderers;

    void onInitializedInternal() override
    {
        registerEntities();
        registerProcessors();
        registerRenderers();
        resolveDependencies();
        onInitialized();
    }

    virtual void onInitialized()
    {

    }

    void onTickInternal(RenderContext& context, float dt) override
    {
        m_clock.restart();
        onTick(dt);
        std::apply([this, dt](auto&&... args) { (args->updateInternal(dt), ...); }, m_processors.hub);
        std::apply([this](auto&&... args) { (removeEntities(*args), ...); }, m_entities.hub);
        if (!m_skip_render) {
            std::apply([this, &context](auto&&... args) { (args->renderInternal(context), ...); }, m_renderers.hub);
        }
        m_execution_time_us = m_clock.getElapsedTime().asMicroseconds();
    }

    template<typename TEntity>
    static void removeEntities(siv::Vector<TEntity>& container)
    {
        container.remove_if([](TEntity& e){ return e.removeRequested(); });
    }

    virtual void onTick(float dt)
    {
        /* By default this does nothing.
         *
         * It can be used to perform things before the tick actually happens.
         */
    }

protected:
    // Entities
    void registerEntities()
    {
        std::apply([this](auto&&... args) { (initializeContainer(args), ...); }, m_entities.hub);
    }

    template<typename T>
    void initializeContainer(ObjectPtr<EntityContainer<T>>& container)
    {
        container = std::make_unique<EntityContainer<T>>();
    }

    // Processors
    void registerProcessors()
    {
        std::apply([this](auto&&... args) { (createProcessor(args), ...); }, m_processors.hub);
    }

    // Renderers
    void registerRenderers()
    {
        std::apply([this](auto&&... args) { (createRenderer(args), ...); }, m_renderers.hub);
    }

    template<typename T>
    void createProcessor(ObjectPtr<T>& p)
    {
        p = std::make_unique<T>();
        initializeSystem(p.get());
    }

    template<typename T>
    void createRenderer(ObjectPtr<T>& p)
    {
        p = std::make_unique<T>(m_render_context->getRenderSize(), m_resources);
        initializeSystem(p.get());
    }

    template<typename T>
    void initializeSystem(T* p)
    {
        p->setScene(this);
        p->loadEntities(m_entities.hub);
        p->loadProcessors(m_processors.hub);
        p->loadRenderers(m_renderers.hub);
    }

    template<typename T, typename... TArgs>
    siv::ID create(TArgs&&... args)
    {
        return m_entities.template create<T>(std::forward<TArgs>(args)...);
    }

    template<typename T>
    T& get(siv::ID const id)
    {
        return m_entities.template get<T>(id);
    }

    template<typename T>
    T const& getConst(siv::ID const id) const
    {
        return m_entities.template get<T>(id);
    }

    template<typename TEntity>
    siv::Handle<TEntity> getHandle(size_t id)
    {
        return m_entities.template getHandle<TEntity>(id);
    }

    template<typename TEntity>
    siv::Handle<TEntity> getHandle(TEntity const& entity)
    {
        return m_entities.template getHandle<TEntity>(entity);
    }

    template<typename TEntity, typename TCallback>
    void foreach(TCallback&& callback)
    {
        auto& container = m_entities.template getContainer<TEntity>();
        for (auto& entity : container) {
            callback(entity);
        }
    }

    template<typename T>
    size_t getCount()
    {
        return m_entities.template getCount<T>();
    }

    template<typename TProcessor>
    TProcessor& getProcessor()
    {
        return m_processors.template get<TProcessor>();
    }

    template<typename TRenderer>
    TRenderer& getRenderer()
    {
        return m_renderers.template get<TRenderer>();
    }

    template<typename T>
    EntityContainer<T>& getContainer()
    {
        return m_entities.template getContainer<T>();
    }

    template<typename T>
    std::vector<T>& getRawContainer()
    {
        return getContainer<T>().getData();
    }

private:
    void resolveDependencies()
    {
        // Sets the internal pointers to dependencies
        std::apply([this](auto&&... args) { (args->loadProcessors(m_processors.hub), ...); }, m_processors.hub);
        std::apply([this](auto&&... args) { (args->loadRenderers(m_renderers.hub), ...); }, m_processors.hub);
        std::apply([this](auto&&... args) { (args->loadProcessors(m_processors.hub), ...); }, m_renderers.hub);
        std::apply([this](auto&&... args) { (args->loadRenderers(m_renderers.hub), ...); }, m_renderers.hub);
        // Systems are now fully initialized
        std::apply([this](auto&&... args) { (args->onInitialized(), ...); }, m_processors.hub);
        std::apply([this](auto&&... args) { (args->onInitialized(), ...); }, m_renderers.hub);
    }

    /// Profiling clock
    sf::Clock m_clock;
    size_t m_execution_time_us{};
};
}  // namespace pez
