#pragma once
#include "standard/widget.hpp"

#include "./ui_common.hpp"
#include "./activity_button.hpp"


struct WindowHeader final : ui::Widget
{
    using Ptr = std::shared_ptr<WindowHeader>;

    static float constexpr s_height = 80.0f;

    Vec2i click_position{};

    std::string title = "Title";

    sf::Font const& font;
    pez::Card background;

    explicit
    WindowHeader(sf::Font const& font_, float const width)
        : ui::Widget{{width, s_height}}
        , font{font_}
        , background{{width, s_height}, 0.0f, {50, 50, 50}}
    {
        background.setShadowSize(20.0f);
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates const states) const override
    {
        target.draw(background, states);

        sf::Text text{font, title, 48};
        auto const bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + Vec2f{0.0f, bounds.size.y * 0.5f});
        text.setPosition({(s_height - bounds.size.y) * 0.5f, s_height * 0.5f});
        text.setFillColor(pez::setAlpha(sf::Color::White, 100));
        target.draw(text, states);
    }

    bool onClick(Vec2f const) override
    {
        click_position = sf::Mouse::getPosition();
        return true;
    }

    [[nodiscard]]
    ActivityButton::Ptr getButton(size_t const idx)
    {
        return getTypedChild<ActivityButton>(idx);
    }
};
