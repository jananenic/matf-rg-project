
#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>
#include <engine/graphics/PointShadows.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_castle();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void set_shader_uniforms(engine::resources::Shader* shader);

    void draw_skybox();

    void draw_grass();

    //void poll_events() override;

    std::vector<glm::vec3> lamp_positions = {
        glm::vec3(0.0f, -1.5f, -4.8f),
        glm::vec3(-0.52f,-1.5f, -4.8f)
    };
    //glm::vec3 lightPos = glm::vec();
    glm::vec3 lightPos = lamp_positions[0];
    glm::vec3 lamp_color = glm::vec3(0.93f, 0.6f, 0.7f);

    glm::vec3 dir_direction =glm::vec3(0.7f, 1.0f, 0.3f);
    glm::vec3 dir_ambient = glm::vec3(0.7f, 0.6f, 0.6f);
    glm::vec3 dir_diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 dir_specular = glm::vec3(1.0f, 1.0f, 1.0f);

    bool m_shadows_enabled = true;
    float m_far_plane = 25.0f;
    engine::graphics::PointShadows m_point_shadows;
    bool lamps_enabled = true;

    float lamp_strength = 1.0f;

    void draw_lamp(glm::vec3 v, float a);

    void draw_lamps();

    void draw_davinci();

    void render_depth(engine::resources::Shader * shader);

    void draw() override;

    void end_draw() override;

    float angle = 0.0f;
    bool da_vinci_enabled = false;
    float speed = 1.0f;
    float timer ;
    float radius = 4.0f;
    bool timer_started;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP
