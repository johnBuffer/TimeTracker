#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"


struct ActivityButton final : ui::Widget
{
    using Ptr = std::shared_ptr<ActivityButton>;

    enum class State
    {
        Active,
        Idle,
    };

    // State
    State state = State::Idle;

    // Render info
    sf::Font const& font;
    pez::CardOutlined background;

    pez::InterpolatedFloat highlight_offset;
    pez::InterpolatedFloat highlight_scale;

    pez::InterpolatedFloat background_height;

    std::function<void()> on_activate;

    explicit
    ActivityButton(sf::Font const& font_, Vec2f const size_)
        : ui::Widget{size_}
        , font{font_}
        , background{ui::createBackground(size_)}
    {
        Vec2f const background_size = background.getOutlineSize();
        background.setOrigin(background_size * 0.5f);
        background.setPosition(background_size);

        highlight_offset.setInterpolationSpeed(5.0f);
        highlight_offset.setValueDirect(0.0f);

        highlight_scale.setInterpolationSpeed(5.0f);
        highlight_scale.setValueDirect(1.0f);

        background_height.setInterpolationSpeed(1.0f);
        background_height.setValueDirect(size->y);
    }

    void onUpdate(float const dt) override
    {
        if (!background_height.isDone()) {
            background.setOuterSize({size->x, background_height});
        }
        float const offset = highlight_offset;
        Vec2f const background_size = *size;
        background.setPosition(background_size * 0.5f + Vec2f{0.0f, offset});
        background.background.shadow_offset.y = 10.0f - offset;
        float const scale = highlight_scale;
        background.setScale({scale, scale});
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        float const text_scale = 0.3f;
        sf::Text text{font, "Active", 256};
        text.setScale({text_scale, text_scale});
        auto const bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + bounds.size * 0.5f);
        text.setPosition({size->x * 0.5f, size->y * 0.7f});
        text.setFillColor(pez::setAlpha(sf::Color::White, 200));
        target.draw(text, states);

        target.draw(background, states);
    }

    bool onClick(Vec2f const) override
    {
        if (state == State::Idle) {
            activate();
        } else {
            deactivate();
        }
        return true;
    }

    void onMouseEnter() override
    {
        if (state == State::Idle) {
            highlight();
        }
    }

    void onMouseExit() override
    {
        resetHighlight();
    }

    void activate()
    {
        state = State::Active;
        background_height = size->y * 0.5f;
        resetHighlight();

        if (on_activate) {
            on_activate();
        }
    }

    void deactivate()
    {
        state = State::Idle;
        background_height = size->y;
    }

private:
    void highlight()
    {
        highlight_offset = -ui::margin * 0.25f;
        highlight_scale = 1.02f;
    }

    void resetHighlight()
    {
        highlight_offset = 0.0f;
        highlight_scale = 1.0f;
    }
};
