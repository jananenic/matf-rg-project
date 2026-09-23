#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/vec3.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void begin_draw() override;
    void render_depth(engine::resources::Shader *shader);

    void set_shader_uniforms(engine::resources::Shader *shader);

    void draw_castle();

    void draw_skybox();

    void draw_grass();

    void draw_lamp(glm::vec3 v, float a);

    void draw_lamps();

    void draw_knight();

    void draw_davinci();

    void update_camera();

    void update() override;

    void draw() override;

    void end_draw() override;

public:
    glm::vec3 lamp_color = glm::vec3(0.93f, 0.6f, 0.7f);


private:
    std::vector<glm::vec3> m_lamp_positions = {
            glm::vec3(0.0f, -1.5f, -4.8f),
            glm::vec3(-0.52f, -1.5f, -4.8f)};
    glm::vec3 m_light_pos = (m_lamp_positions[0] + m_lamp_positions[1]) / glm::vec3(2);

    bool m_lamps_enabled = true;
    float m_lamp_strength = 1.0f;
    float m_angle = 0.0f;
    bool m_da_vinci_enabled = false;
    float m_timer;
    float m_radius = 4.0f;
    bool m_timer_started;

public:
    std::string_view name() const override { return "app::MainController"; }
};

}// namespace app

#endif//MAINCONTROLLER_HPP