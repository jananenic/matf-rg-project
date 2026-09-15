//
// Created by matfrg on 9/10/26.
//

#ifndef GRAYSCALECONTROLLER_HPP
#define GRAYSCALECONTROLLER_HPP

#include <cstdint>
#include <engine/core/Controller.hpp>

namespace engine::resources {
class Shader;
};

namespace engine::graphics {

class PostProcessingController : public core::Controller {
public:
    std::string_view name() const override {
        return "engine::graphics::GrayscaleController";
    }


protected:
    void initialize() override;

    void begin_draw() override;

    void present(const resources::Shader *shader) const;//ime

    void terminate() override;

private:
    void resize(int width, int height);

    void begin_capture() const;

    void destroy();

    void initialize_screen_quad();

    unsigned int m_framebuffer = 0;
    unsigned int m_color_texture = 0;
    unsigned int m_rbo = 0;
    unsigned int m_quad_vao = 0;
    unsigned int m_quad_vbo = 0;

    int m_width = 0;
    int m_height = 0;
};

}// namespace engine::graphics

#endif//GRAYSCALECONTROLLER_HPP
