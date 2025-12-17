#pragma once
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "button.hpp"
#include "widget.hpp"

/// The screen's side on which the Drawer will be located
enum class Side
{
    Top,
    Right,
    Bottom,
    Left
};


struct DrawerHandle final : public Button
{
    using Ptr = std::shared_ptr<DrawerHandle>;

    pez::InterpolatedFloat scale;

    DrawerHandle(sf::Color const color, std::string const& label, sf::Font const& font)
        : Button(color, label, font)
    {
        scale.setValueDirect(1.0f);
        scale.setInterpolation(pez::InterpolationFunction::EaseOutBack, 3.0f);
        scale.useRealtime();
    }

    void onUpdate(float const) override
    {
        setScale({scale, scale});
    }

    void onMouseEnter() override
    {
        scale = 1.2f;
    }

    void onMouseExit() override
    {
        scale = 1.0f;
    }
};


template<typename TWidget>
struct Drawer final : ui::Widget
{
    /// Shorthand
    using Ptr = std::shared_ptr<Drawer>;

    /// The widget embedded in the drawer
    std::shared_ptr<TWidget> widget;
    /// The control button
    DrawerHandle::Ptr button;
    /// The button height
    float pull_length{};
    /// Drawer visibility state
    bool visible = false;
    /// The current drawer position
    pez::InterpolatedVec2 position;
    /// The drawer's Side
    Side side;
    /// The position on the side
    float side_position;
    /// Called when the drawer's state changes (hidden <-> visible)
    std::function<void(bool)> on_state_change_callback;

    /// Constructor that directly builds the managed widget
    template<typename ...TArgs>
    Drawer(Side const side_, float const side_position_, sf::Font const& font, std::string const& label, TArgs&&... args)
        : side{side_}
        , side_position{side_position_}
    {
        button = createChild<DrawerHandle>(sf::Color::White, label, font);

        pull_length = button->size->y - button->radius;
        widget = createChild<TWidget>(std::forward<TArgs>(args)...);

        position.setInterpolationSpeed(2.0f);
        position.useRealtime();

        updateSize();
        initializeWidget();
        initializeButton();
    }

    void setColor(sf::Color const color) const
    {
        button->background.setColor(color);
    }

    void setDrawState(bool const draw_state)
    {
        if (draw_state == visible) {
            return;
        }
        position = draw_state ? m_position_visible : m_position_hidden;
        visible = draw_state;
        tryCallOnStateChange();
    }

    void initializeControls(Vec2f const ui_size)
    {
        m_position_hidden = getHiddenPosition(ui_size);
        m_position_visible = getVisiblePosition(ui_size);
        position.setValueDirect(visible ? m_position_visible : m_position_hidden);

        button->setOnClickCallback([=, this]
        {
            button->onMouseExit();
            setDrawState(!visible);
        });
    }

    void onUpdate(float const) override
    {
        setPosition(position);
        if (position.isDone() && !visible) {
            widget->setVisible(false);
        } else {
            widget->setVisible(true);
        }
    }

    void setSidePosition(float const side_position_)
    {
        side_position = side_position_;
        initializeWidget();
        initializeButton();
    }

private:
    Vec2f m_position_hidden;
    Vec2f m_position_visible;

    void initializeWidget()
    {
        switch (side) {
            case Side::Top:
                widget->setPosition({});
                break;
            case Side::Right:
                widget->setPosition({pull_length, 0.0f});
                break;
            case Side::Bottom:
                widget->setPosition({0.0f, pull_length});
                break;
            case Side::Left:
                widget->setPosition({});
                break;
        }
    }

    void initializeButton()
    {
        button->setOrigin({button->size->x * 0.5f, button->size->y - button->radius});
        button->setLabelOffset(-6.0f);

        Vec2f const size_value{*size};
        Vec2f const widget_center{widget->getCenterPosition()};

        switch (side) {
            case Side::Top:
                button->setRotation(sf::degrees(0));
                button->setPosition({widget_center.x, widget->size->y});
                button->setOrigin({button->size->x * 0.5f, button->radius});
                button->setLabelOffset(6.0f);
                break;
            case Side::Right:
                button->setRotation(sf::degrees(-90));
                button->setPosition({pull_length, widget_center.y});
                break;
            case Side::Bottom:
                button->setRotation(sf::degrees(0));
                button->setPosition({widget_center.x, pull_length});
                button->setOrigin({button->size->x * 0.5f, button->size->y - button->radius});
                break;
            case Side::Left:
                button->setRotation(sf::degrees(90));
                button->setPosition({size_value.x - pull_length, widget_center.y});
                break;
        }
    }

    void updateSize()
    {
        switch (side) {
            case Side::Top:
            case Side::Bottom:
                size = {widget->size->x, widget->size->y + pull_length};
                break;
            case Side::Left:
            case Side::Right:
                size = {widget->size->x + pull_length, widget->size->y};
                break;
        }
    }

    [[nodiscard]]
    Vec2f getHiddenPosition(Vec2f const ui_size) const
    {
        if (!widget->size) {
            return {};
        }
        Vec2f const scale = getScale();
        Vec2f const size_value{widget->size->componentWiseMul(scale)};
        switch (side) {
            case Side::Top:
                return {side_position, -size_value.y};
            case Side::Right:
                return {ui_size.x - pull_length * scale.x, side_position};
            case Side::Bottom:
                return {side_position, ui_size.y - pull_length * scale.y};
            case Side::Left:
                return {-size_value.x, side_position};
            default:
                return {};
        }
    }

    [[nodiscard]]
    Vec2f getVisiblePosition(Vec2f const ui_size) const
    {
        if (!widget->size) {
            return {};
        }

        Vec2f const hidden_position = getHiddenPosition(ui_size);
        Vec2f const scale = getScale();
        Vec2f const size_value{widget->size->componentWiseMul(scale)};

        switch (side) {
            case Side::Top:
                return hidden_position + Vec2f{0.0f, size_value.y + ui::margin};
            case Side::Right:
                return hidden_position - Vec2f{size_value.x + ui::margin, 0.0f};
            case Side::Bottom:
                return hidden_position - Vec2f{0.0f, size_value.y + ui::margin};
            case Side::Left:
                return hidden_position + Vec2f{size_value.x + ui::margin, 0.0f};
            default:
                return {};
        }
    }

    void tryCallOnStateChange() const
    {
        if (on_state_change_callback) {
            on_state_change_callback(visible);
        }
    }
};