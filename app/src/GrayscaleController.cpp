//
// Created by matfrg on 9/10/26.
//

#include <../include/GrayscaleController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {

void GrayscaleController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if(platform->key(engine::platform::KEY_C).state() == engine::platform::Key::State::JustPressed) {
        grayscale_enabled = !grayscale_enabled;
    }
}

void GrayscaleController::draw() {

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("grayscale");
    shader->use();
    shader->set_bool("grayscale_enabled", grayscale_enabled);
    present(shader);

}
} // engine