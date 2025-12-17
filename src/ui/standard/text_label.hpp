#pragma once
#include "./widget.hpp"
#include "peztool/utils/interpolation/standard_interpolated_value.hpp"
#include "peztool/utils/render/card/card_outlined.hpp"


struct TextLabel final : ui::Widget
{
    using Ptr = std::shared_ptr<TextLabel>;

    explicit
    TextLabel(sf::Font const& font)
        : ui::Widget{}
        , m_text{font, "", 32}
    {
        m_text.setScale(s_quality_scale_vec_inv);
    }

    void setString(std::string const& str)
    {
        m_text.setString(str);
        updateSize();
    }

    void setCharacterSize(int32_t const char_size)
    {
        m_text.setCharacterSize(static_cast<int32_t>(static_cast<float>(char_size) * s_quality_scale));
        updateSize();
    }

    void setFillColor(sf::Color const color)
    {
        m_text.setFillColor(color);
    }

    void onUpdate(float const) override
    {
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_text, states);
    }

private:
    static constexpr float s_quality_scale = 4.0f;
    static constexpr float s_quality_scale_inv = 1.0f / s_quality_scale;
    static constexpr Vec2f s_quality_scale_vec = {s_quality_scale, s_quality_scale};
    static constexpr Vec2f s_quality_scale_vec_inv = {s_quality_scale_inv, s_quality_scale_inv};

    sf::Text m_text;

    void updateSize()
    {
        auto const bounds = m_text.getLocalBounds();
        size = bounds.size * s_quality_scale_inv;
        m_text.setOrigin(bounds.position);
    }
};