#pragma once
#include "radio_button.hpp"


struct RadioGroup final
{
    using Ptr = std::shared_ptr<RadioGroup>;
    using UpdateCallback = std::function<void(size_t)>;
    std::vector<RadioButton::Ptr> buttons;

    UpdateCallback on_change_callback;
    size_t current_choice{0};

    explicit
    RadioGroup(std::vector<RadioButton::Ptr>&& buttons_, size_t const default_choice = 0)
    {
        setButtons(std::move(buttons_), default_choice);
    }

    void setButtons(std::vector<RadioButton::Ptr>&& buttons_, size_t const default_choice = 0)
    {
        buttons = std::move(buttons_);

        size_t idx{0};
        for (auto const& b : buttons) {
            b->setSelectCallback([=, this]
            {
                select(idx);
            });
            ++idx;
        }

        select(default_choice);
    }

    void select(size_t const choice)
    {
        if (!buttons[choice]->state) {
            buttons[current_choice]->setState(false);
            current_choice = choice;
            buttons[current_choice]->setState(true);
            if (on_change_callback) {
                on_change_callback(current_choice);
            }
        }
    }
};
