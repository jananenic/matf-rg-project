//
// Created by matfrg on 9/2/26.
//

#include "GuiController.hpp"

#include <imgui.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>

namespace app {
void GuiController::initialize() {
    set_enable(false);
}

void GuiController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}

void GuiController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->begin_gui();

    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::Text("Press L to turn off/on the lights\n");
    //ImGui::Text("Press F to see da vincis flying machine fly\n");
    ImGui::Text("Press C to remove colors from the screen\n");
    ImGui::End();
    graphics->end_gui();
}
} // app