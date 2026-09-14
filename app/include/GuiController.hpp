//
// Created by matfrg on 9/2/26.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class GuiController : public engine::core::Controller {
public:
    std::string_view name() const override { return "app::GUIController"; }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};

}// namespace app

#endif//GUICONTROLLER_HPP