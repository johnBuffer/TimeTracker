#pragma once
#include <memory>
#include "peztool/utils/vec.hpp"
#include "origin.hpp"

namespace ui
{

struct Widget : sf::Drawable, sf::Transformable
{
    /// Shorthand for handle to widget
    using Ptr = std::shared_ptr<Widget>;

    /// The widget's size
    std::optional<Vec2f> size = std::nullopt;
    /// Children widgets
    std::vector<Ptr> children;
    /// Currently hovered child
    Ptr active = nullptr;

    /// Default constructor
    Widget() = default;

    /// Constructor
    explicit
    Widget(Vec2f const size_)
        : size{size_}
    {}

    void attach(Ptr const& child)
    {
        children.push_back(child);
    }

    template<typename TWidget>
    std::shared_ptr<TWidget> attachTyped(std::shared_ptr<TWidget> const& child)
    {
        children.push_back(child);
        return child;
    }

    void setOriginMode(origin::Mode const origin)
    {
        setOrigin(getOriginPosition(origin));
    }

    [[nodiscard]]
    Vec2f getOriginPosition(origin::Mode const origin) const
    {
        if (!size) {
            std::cout << "Cannot set origin mode, no size has been provided" << std::endl;
            return {};
        }
        Vec2f const size_value = *size;
        return origin::getPosition(origin, size_value);
    }

    /// The top level draw method, should not be called manually
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        if (!m_visible) {
            return;
        }
        // Apply local transform
        states.transform *= getTransform();
        // Draw this widget first
        onDraw(target, states);
        // Then draw its children
        for (auto& child : children) {
            child->draw(target, states);
        }
    }

    /// The widget specific draw
    virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const
    {

    }

    /// Checks if the provided position is within the widget
    [[nodiscard]]
    bool contains(Vec2f const pos) const
    {
        sf::FloatRect const bounding_box{{}, size.value_or(Vec2f{})};
        return bounding_box.contains(getLocalPoint(pos));
    }

    void mouseEnter()
    {
        onMouseEnter();
    }

    void mouseExit()
    {
        if (active) {
            active->mouseExit();
            active = nullptr;
        }
        onMouseExit();
    }

    void mouseMove(Vec2f const pos)
    {
        Vec2f const local_pos = getLocalPoint(pos);
        onMouseMove(local_pos);

        // Check if there is an active widget
        if (active) {
            // Ensure it is still active
            if (active->contains(local_pos) || m_descendant_clicked) {
                active->mouseMove(local_pos);
            } else {
                active->mouseExit();
                active = nullptr;
            }
        }

        // Not using an else because active may have been invalidated during previous if
        if (!active) {
            // Currently not taking order into account. Should be last checked first but I will
            // assume that there is no overlap
            for (auto const& child : children) {
                if (child->contains(local_pos)) {
                    active = child;
                    active->mouseEnter();
                    active->mouseMove(local_pos);
                    break;
                }
            }
        }
    }

    bool click(Vec2f const pos)
    {
        Vec2f const local_pos = getLocalPoint(pos);
        if (active) {
            m_descendant_clicked = active->click(local_pos);
            if (m_descendant_clicked) {
                return true;
            }
        }
        m_clicked = onClick(local_pos);
        return m_clicked;
    }

    void unclick(Vec2f const pos)
    {
        Vec2f const local_pos = getLocalPoint(pos);
        if (active) {
            active->unclick(local_pos);
            m_descendant_clicked = false;
        }
        if (m_clicked) {
            m_clicked = false;
            onUnclick(local_pos);
        }
        mouseMove(pos);
    }

    /** Called when the mouse moved inside the widget
     *
     * @param pos The local mouse position (relative to the widget's position)
     */
    virtual void onMouseMove(Vec2f const pos)
    {

    }

    virtual void onMouseEnter()
    {

    }

    virtual void onMouseExit()
    {

    }

    /**
     *
     * @param pos The position relative to the widget's position
     * @return A flag telling if the click has been consumed by the widget
     */
    [[nodiscard]]
    virtual bool onClick(Vec2f const pos)
    {
        return false;
    }

    virtual void onUnclick(Vec2f const pos)
    {

    }

    template<typename TWidget, typename... Args>
    std::shared_ptr<TWidget> createChild(Args&&... args)
    {
        auto w = std::make_shared<TWidget>(std::forward<Args>(args)...);
        attach(w);
        return w;
    }

    [[nodiscard]]
    Vec2f getCenterPosition() const
    {
        return getPosition() + size.value_or(Vec2f{}) * 0.5f - getOrigin();
    }

    void update(float const dt)
    {
        onUpdate(dt);
        for (auto const& child : children) {
            child->update(dt);
        }
    }

    virtual void onUpdate(float const dt)
    {

    }

    [[nodiscard]]
    bool isClicked() const
    {
        return m_clicked;
    }

    void setVisible(bool const visible)
    {
        m_visible = visible;
    }

    void setChildrenVisibility(bool const visible) const
    {
        for (auto const& child : children) {
            child->setVisible(visible);
        }
    }

    [[nodiscard]]
    Vec2f getEffectiveSize() const
    {
        if (!size) {
            return {};
        }
        return size->componentWiseMul(getScale());
    }

    template<typename TWidget>
    std::shared_ptr<TWidget> getTypedChild(size_t const idx)
    {
        if (idx < children.size()) {
            return std::dynamic_pointer_cast<TWidget>(children[idx]);
        }
        return nullptr;
    }

private:
    bool m_clicked = false;
    bool m_descendant_clicked = false;
    bool m_visible = true;

    [[nodiscard]]
    Vec2f getLocalPoint(Vec2f const point) const
    {
        return getTransform().getInverse().transformPoint(point);
    }
};

}
