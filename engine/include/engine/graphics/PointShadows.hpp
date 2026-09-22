
#ifndef POINTSHADOWS_HPP
#define POINTSHADOWS_HPP

#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace engine::graphics {

class PointShadows {
public:
    PointShadows();
    ~PointShadows();

    void initialize(unsigned int shadow_width = 1024, unsigned int shadow_height = 1024);

    void begin(const glm::vec3& light_pos, engine::resources::Shader* depth_shader);

    void end(int screen_width, int screen_height);

    unsigned int depth_cubemap() const {return  m_depth_cubemap;}

    void bind_depth_map(unsigned int texture_unit = 0) const;

    float far_plane() const {return m_far_plane;}

    void terminate();

private:
    unsigned int m_shadow_fbo = 0;
    unsigned int m_depth_cubemap = 0;
    unsigned int m_shadow_width = 1024;
    unsigned int m_shadow_height = 1024;
    float m_near_plane = 0.1f;
    float m_far_plane = 25.0f;


};

} // engine

#endif //POINTSHADOWS_HPP