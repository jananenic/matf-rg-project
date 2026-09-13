
#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_castle();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw_grass();

    //void poll_events() override;

    std::vector<glm::vec3> lamp_positions = {
        glm::vec3(0.0f, -1.5f, -4.8f),
        glm::vec3(-0.52f,-1.5f, -4.8f)
    };

    glm::vec3 lamp_color = glm::vec3(0.93f, 0.6f, 0.7f);

    bool lamps_enabled = true;

    float lamp_strength = 0.7f;

    void draw_lamp(glm::vec3 v, float a);

    void draw_lamps();

    void draw_davinci();

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP
