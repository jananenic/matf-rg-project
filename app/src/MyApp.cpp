//
// Created by matfrg on 8/30/26.
//

#include <GrayscaleController.hpp>
#include <GuiController.hpp>
#include <MainController.hpp>
#include <MyApp.hpp>
#include <spdlog/spdlog.h>

namespace app {

void MyApp::app_setup() {
    spdlog::info("App setup completed!");
    auto main_controller = register_controller<app::MainController>();
    auto gui_controller = register_controller<GuiController>();
    auto grayscale_controller = register_controller<GrayscaleController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    grayscale_controller->after(main_controller);
    grayscale_controller->before(gui_controller);
}
}// namespace app