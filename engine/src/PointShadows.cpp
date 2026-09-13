//
// Created by matfrg on 9/8/26.
//

#include <engine/graphics/PointShadows.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


namespace engine::graphics {
PointShadows::PointShadows() = default;

PointShadows::~PointShadows() = default;

void PointShadows::initialize(unsigned int shadow_width, unsigned int shadow_height) {
    m_shadow_width = shadow_width;
    m_shadow_height = shadow_height;

    glGenFramebuffers(1, &m_shadow_fbo);

    glGenTextures(1, &m_depth_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     m_shadow_width, m_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void PointShadows::begin(const glm::vec3& light_pos,  engine::resources::Shader *depth_shader) {

    //initialize();
    glViewport(0, 0, m_shadow_width, m_shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);




    float aspect = (float)m_shadow_width / (float)m_shadow_height;
    glm :: mat4 shadow_projection = glm::perspective(glm::radians(90.0f),aspect,m_near_plane,m_far_plane);


    std::vector<glm::mat4> shadow_transforms;
    shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    depth_shader->use();
    for(unsigned int i = 0; i < 6; ++i) {
        depth_shader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
    }
    depth_shader->set_float("far_plane", m_far_plane);
    depth_shader->set_vec3("lightPos", light_pos); //?

    //snimak
    //glBindFramebuffer(GL_FRAMEBUFFER,0);


    //!!!!!!!!!proveri
    /*
    glViewport(0,0, m_shadow_width, m_shadow_height);//src width height
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    */
}

void PointShadows::end(int screen_width, int screen_height) {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0, 0, screen_width, screen_height);
}

void PointShadows::bind_depth_map(unsigned int texture_unit) const {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
}

void PointShadows::terminate() {
    if(m_shadow_fbo) glDeleteBuffers(1, &m_shadow_fbo);
    if(m_depth_cubemap) glDeleteTextures(1, &m_depth_cubemap);
}
} // engine