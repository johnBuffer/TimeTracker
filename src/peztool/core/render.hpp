#pragma once
#include <cassert>
#include "SFML/Graphics.hpp"

#include "../utils/vec.hpp"
#include "../utils/events.hpp"


namespace pez
{

/// Represents a render layer used to sort draw calls and apply different transforms to objects
struct Layer
{
    /// Identifies the layer in the layer container
    using ID = uint64_t;

    /// The target texture where objects will be drawn
    sf::RenderTarget* texture_ptr;
    sf::Transform transform;

    bool transform_changed{true};
    Vec2f const center;
    float scale = 1.0f;
    Vec2f render_scale{1.0f, 1.0f};
    Vec2f offset;

    explicit
    Layer(Vec2u const size_, sf::RenderTarget& target_)
        : texture_ptr{&target_}
        , center{static_cast<Vec2f>(size_) * 0.5f}
    {
    }

    void moveView(sf::Vector2f v)
    {
        offset += v / scale;
        transform_changed = true;
    }

    void zoom(float zoom)
    {
        scale *= zoom;
        transform_changed = true;
    }

    void setViewPosition(Vec2f position)
    {
        offset = position;
        transform_changed = true;
    }

    void setZoom(float zoom)
    {
        scale = zoom;
        transform_changed = true;
    }

    [[nodiscard]]
    sf::FloatRect getViewport(float margin = 0.0f) const
    {
        Vec2f const size{(2.0f / scale) * center + 2.0f * Vec2f{margin, margin}};
        return {offset - size * 0.5f, size};
    }

    [[nodiscard]]
    float getZoom() const
    {
        return scale;
    }

    /// Draws the object on the texture, applying the transform
    void draw(sf::Drawable const& drawable)
    {
        texture_ptr->draw(drawable, getTransform());
    }

    /// Draws the object on the texture, applying the transform
    void draw(sf::Drawable const& drawable, sf::RenderStates states)
    {
        states.transform = getTransform() * states.transform;
        texture_ptr->draw(drawable, states);
    }

    sf::Transform const& getTransform()
    {
        if (transform_changed) {
            transform_changed = false;
            transform = {};
            transform.translate(center);
            transform.scale(render_scale * scale);
            transform.translate(-offset);
        }
        return transform;
    }
};

/// Stores all layers and routes draw calls to relevant layers, applying the corresponding transformations
class RenderContext
{
public:
    /// RenderContext
    explicit
    RenderContext(sf::RenderWindow& window, sf::Vector2u const size)
        : m_window{window}
        , m_size{size}
        , m_window_size{m_window.getSize()}
        , m_size_f{static_cast<sf::Vector2f>(size)}
    {
        sf::ContextSettings settings;
        settings.antiAliasingLevel = 4;
        if (!m_render_texture.resize(size, settings)) {
            std::cout << "Cannot create render texture" << std::endl;
        } else {
            m_render_texture.setSmooth(true);
        }
        updateMousePositionScale();
        updateMousePosition();
        m_scale = {static_cast<float>(m_window_size.x) / m_size_f.x,
                   static_cast<float>(m_window_size.y) / m_size_f.y};
    }

    /// Creates a new layer and registers it
    Layer::ID registerLayer()
    {
        m_layers.emplace_back(m_size, m_render_texture);
        m_layers.back().render_scale = m_scale;
        return m_layers.size() - 1;
    }

    /// Draw directly to the window, skips layers
    void draw(sf::Drawable const& drawable)
    {
        sf::Transform transform;
        transform.scale(m_scale);
        draw(drawable, transform);
    }

    /// Draw directly to the window, skips layers
    void draw(sf::Drawable const& drawable, sf::RenderStates const& states)
    {
        m_render_texture.draw(drawable, states);
    }

    /// Dispatch the draw call to the target Layer
    void draw(sf::Drawable const& drawable, Layer::ID const layer)
    {
        assert(layer < m_layers.size());
        m_layers[layer].draw(drawable);
    }

    /// Dispatch the draw call to the target Layer
    void draw(sf::Drawable const& drawable, sf::RenderStates const states, Layer::ID const layer)
    {
        assert(layer < m_layers.size());
        m_layers[layer].draw(drawable, states);
    }

    /// Creates a world layer and a HUD layer. The world layer has its viewport controlled by mouse.
    void createDefaultLayers(EventHandler& handler)
    {
        // Create the layers
        m_world_layer = registerLayer();
        m_hud_layer = registerLayer();

        float constexpr zoom_factor     = 1.2f;
        float constexpr zoom_factor_inv = 1.0f / zoom_factor;
        handler.onMouseWheelScrolled([this](sf::Event::MouseWheelScrolled const& event) {
            // Ensure that there is actually a delta (many 0s on MacOS)
            if (event.delta == 0.0f) {
                return;
            }
            Layer& world_layer = m_layers[m_world_layer];
            if (event.delta > 0.0f) {
                world_layer.zoom(zoom_factor);
            } else {
                world_layer.zoom(zoom_factor_inv);
            }
        });

        handler.onMouseMoved([this](sf::Event::MouseMoved const&) {
            Layer& world_layer = m_layers[m_world_layer];
            Vec2f const old_position = m_mouse_position;
            updateMousePosition();
            if (mouse_clicked) {
                world_layer.moveView(old_position - m_mouse_position);
            }

        });
    }

    [[nodiscard]]
    Layer::ID getWorldLayerID() const
    {
        return m_world_layer;
    }

    [[nodiscard]]
    Layer::ID getHudLayerID() const
    {
        return m_hud_layer;
    }

    [[nodiscard]]
    Layer& getLayer(Layer::ID const id)
    {
        assert(id < m_layers.size());
        return m_layers[id];
    }

    [[nodiscard]]
    Layer const& getLayer(Layer::ID const id) const
    {
        assert(id < m_layers.size());
        return m_layers[id];
    }

    [[nodiscard]]
    Layer& getWorldLayer()
    {
        return getLayer(getWorldLayerID());
    }

    [[nodiscard]]
    Layer const& getWorldLayer() const
    {
        return getLayer(getWorldLayerID());
    }

    [[nodiscard]]
    Layer& getHudLayer()
    {
        return getLayer(getHudLayerID());
    }

    void clear()
    {
        m_render_texture.clear(m_clear_color);
    }

    void renderLayers()
    {
        m_render_texture.display();
        sf::Sprite const render_sprite{m_render_texture.getTexture()};
        m_window.draw(render_sprite);
        m_window.display();
    }

    [[nodiscard]]
    Vec2f getRenderSize() const
    {
        return m_size_f;
    }

    [[nodiscard]]
    Vec2f getMousePosition() const
    {
        return m_mouse_position;
    }

    [[nodiscard]]
    Vec2f getMouseWorldPosition() const
    {
        Layer const& world_layer = getWorldLayer();
        Vec2f const mouse_vec{m_mouse_position - m_size_f * 0.5f};
        return mouse_vec.componentWiseDiv(m_scale * world_layer.scale) + world_layer.offset;
    }

    void updateMousePositionScale()
    {
        Vec2f const window_size_f{m_window.getSize()};
        m_mouse_position_coef = m_size_f.componentWiseDiv(window_size_f);
        updateMousePosition();
    }

    sf::Texture const& getTexture()
    {
        m_render_texture.display();
        return m_render_texture.getTexture();
    }

    void setClearColor(sf::Color const color)
    {
        m_clear_color = color;
    }

    [[nodiscard]]
    sf::Window const& getWindow() const
    {
        return m_window;
    }

    /// The mouse button state
    bool mouse_clicked = false;

private:
    /// The window
    sf::RenderWindow& m_window;
    /// The render texture, used for shaders
    sf::RenderTexture m_render_texture;
    /// Stores all the layers
    std::vector<Layer> m_layers;
    /// Render size
    sf::Vector2u m_size;
    /// Window size
    sf::Vector2u m_window_size;
    /// Float version of the render size
    sf::Vector2f m_size_f;
    /// Scale factor
    sf::Vector2f m_scale;
    /// Default world layer
    Layer::ID m_world_layer = 0;
    /// Default HUD layer
    Layer::ID m_hud_layer = 0;
    /// Mouse position
    Vec2f m_mouse_position;
    /// Coefficient to convert window mouse position to render mouse position
    Vec2f m_mouse_position_coef;
    /// The color used to clear the render texture
    sf::Color m_clear_color = sf::Color::Black;

    void updateMousePosition()
    {
        m_mouse_position = Vec2f{sf::Mouse::getPosition(m_window)}.componentWiseMul(m_mouse_position_coef);
    }
};

}
