//
// Created by matfrg on 8/31/26.
//

#include "../include/MainController.hpp"

#include "../../engine/libs/glfw/include/GLFW/glfw3.h"

#include <GuiController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/PointShadows.hpp>
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
        engine::resources::Model* castle = resources->model("castle");

        //shader
        engine::resources::Shader* shader = resources->shader("light");
        set_shader_uniforms(shader);

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
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float dt = platform->dt();
        angle += dt;

        //auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KEY_L).state() == engine::platform::Key::State::JustPressed) { lamps_enabled = !lamps_enabled; }
        if(platform->key(engine::platform::KEY_V).state() == engine::platform::Key::State::JustPressed) {
            timer_started = true;
            timer = 0.0f;

        }
        if(timer_started) {
            timer+=dt;
            if(timer >= 2.0f && timer < 8.0f) {
                da_vinci_enabled = true;
            }
            if(timer >= 8.0f) {
                da_vinci_enabled = false;
                timer_started = false;
            }
        }

    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();

    }

    void MainController::set_shader_uniforms(engine::resources::Shader* shader) {

        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_vec3("viewPos", graphics->camera()->Position);


        shader->set_vec3("dirLight.direction",dir_direction );//
        shader->set_vec3("dirLight.ambient", dir_ambient);//
        shader->set_vec3("dirLight.diffuse", dir_diffuse);//
        shader->set_vec3("dirLight.specular", dir_specular);//

        shader->set_vec3( "pointPos", lamp_positions[0]);
        shader->set_float("constant", 1.0f);
        shader->set_float( "linear", 0.09f);
        shader->set_float("quadratic", 0.6f);
        shader->set_vec3("ambientP", lamp_color * 0.05f * lamp_strength);
        shader->set_vec3( "diffuseP", lamp_color * lamp_strength);
        shader->set_vec3( "specularP", lamp_color * 0.15f * lamp_strength);
        shader->set_bool("light_enabled", lamps_enabled);


        shader->set_int("depthMap", 10);
        graphics->point_shadow()->bind_depth_map(10);
        shader->set_vec3("lightPos", lightPos);
        shader->set_float("far_plane", graphics->point_shadow()->far_plane());
        shader->set_bool("shadows",lamps_enabled);

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
        engine::resources::Model* grass = resources->model("grass");

        //shader
        engine::resources::Shader* shader = resources->shader("light");
        set_shader_uniforms(shader);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.45f, -10.0f));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);
        grass->draw(shader);
    }


    void MainController::draw_lamp(glm::vec3 move, float angle) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Model* lamp = resources->model("lamp");
        //shader
        engine::resources::Shader* shader = resources->shader("light");
        set_shader_uniforms(shader);

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

    void MainController::draw_davinci() {
        if (!da_vinci_enabled) return;
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Model* da_vinci = resources->model("da_vinci");

        //shader
        engine::resources::Shader* shader = resources->shader("light");
        set_shader_uniforms(shader);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.35,1.50,-10.0));
        //radius*cos, angle/rad, radius*sin
        model = glm::translate(model, glm::vec3(cos(angle)*radius, 0, sin(angle)*radius));
        model = glm::rotate(model,glm::radians(90.0f),glm::vec3(0.0,1.0,0));
        model = glm::rotate(model,-angle,glm::vec3(0.0,1.0,0));
        model = glm::scale(model, glm::vec3(0.03f));
        shader->set_mat4("model", model);
        da_vinci->draw(shader);
    }

    void MainController::render_depth(engine::resources::Shader *shader) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

        shader->use();
        //grass
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.45f, -10.0f));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);
        resources->model("grass")->draw(shader);

        //castle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, -10.0f));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));
        model = glm::scale(model, glm::vec3(0.2f));
        shader->set_mat4("model", model);
        resources->model("castle")->draw(shader);

        //lamp
        model = glm::mat4(1.0f);
        model = glm::translate(model, lamp_positions[0]);
        model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
        model = glm::scale(model, glm::vec3(0.03f));
        shader->set_mat4("model", model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lamp_positions[1]);
        model = glm::rotate(model,glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
        model = glm::scale(model, glm::vec3(0.03f));
        shader->set_mat4("model", model);



    }

    void MainController::draw() {

        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        auto depthShader = resources->shader("point_shadows");
        if(depthShader) {
            graphics->point_shadow()->begin(lightPos, depthShader);
            render_depth(depthShader);
            int screenWidth = platform->window()->width();
            int screenHeight = platform->window()->height();
            graphics->point_shadow()->end(screenWidth, screenHeight);


        }

        draw_castle();
        draw_grass();
        draw_lamps();
        draw_davinci();
        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
} // app