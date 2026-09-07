//
// Created by matfrg on 8/31/26.
//

#include "../include/MainController.hpp"

#include "../../engine/libs/glfw/include/GLFW/glfw3.h"

#include <GuiController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if(!gui_controller->is_enabled()) {
            auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
            camera->rotate_camera(position.dx*0.4,position.dy*0.4);
        }
    }

    void MainController::initialize() {

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        engine::graphics::OpenGL::enable_depth_testing();

        //spdlog::info("MainController initialized");

    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::draw_castle() {
        //model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* castle = resources->model("castle");

        //shader
        engine::resources::Shader* shader = resources->shader("light");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, -10.0f));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));
        model = glm::scale(model, glm::vec3(0.2f));
        shader->set_mat4("model", model);
        castle->draw(shader);
    }

    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if(gui_controller->is_enabled()) {
            return;
        }
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        float dt = platform->dt();
        if(platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
        }
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();

    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox =  resources->skybox("skybox_night");
        auto shader = resources->shader("skybox");
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw_grass() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* grass = resources->model("grass");

        //shader
        engine::resources::Shader* shader = resources->shader("light");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        shader->set_vec3("viewPos", graphics->camera()->Position);
        shader->set_float("material.ambient", 0.3f);
        /*
        shader->set_float("material.ambient", 2.3f);
        shader->set_float("material.diffuse", 1.0f);
        shader->set_float("material.specular", 0.0f);
        shader->set_float("material.shininess", 2.0f);
        */
        shader->set_vec3("dirLight.direction", glm::vec3(0.7f, 1.0f, 0.3f));
        shader->set_vec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.45f, -10.0f));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);
        grass->draw(shader);
    }


    void MainController::draw_lamp(glm::vec3 move, float angle) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* lamp = resources->model("lamp");

        //shader
        engine::resources::Shader* shader = resources->shader("light");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        shader->set_vec3("viewPos", graphics->camera()->Position);
        shader->set_float("material.ambient", 0.3f);

        shader->set_vec3("dirLight.direction", glm::vec3(0.7f, 1.0f, 0.3f));
        shader->set_vec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));


        for(int i = 0; i < lamp_positions.size(); i++) {
            std::string uniform = "pointLights[" + std::to_string(i) + "].";
            shader->set_vec3(uniform + "position", lamp_positions[i]);
            shader->set_float(uniform + "constant", 1.0f);
            shader->set_float(uniform + "linear", 0.09f);
            shader->set_float(uniform +"quadratic", 0.6f);
            shader->set_vec3(uniform + "ambient", lamp_color * 0.05f * lamp_strength);
            shader->set_vec3(uniform + "diffuse", lamp_color * lamp_strength);
            shader->set_vec3(uniform + "specular", lamp_color * 0.15f * lamp_strength);
            if(lamps_enabled) {
                shader->set_bool("light_enabled", true);
            }
            else {
                shader->set_bool(  "light_enabled", false);
            }
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, move);
        model = glm::rotate(model,angle,glm::vec3(0.0,1.0,0.0));
        model = glm::scale(model, glm::vec3(0.03f));
        shader->set_mat4("model", model);
        lamp->draw(shader);
    }

    void MainController::draw_lamps() {
        auto vec1 = glm::vec3(0.0f, -2.1f,-5.0f);
        auto angle1 = glm::radians(-90.0f);
        draw_lamp(vec1, angle1);

        auto vec2 = glm::vec3(-0.52f, -2.1f,-5.0f);
        auto angle2 = glm::radians(90.0f);
        draw_lamp(vec2, angle2);




    }

    void MainController::draw() {
        draw_castle();
        draw_grass();
        draw_lamps();
        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
} // app