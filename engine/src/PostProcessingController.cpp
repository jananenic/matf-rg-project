//
// Created by matfrg on 9/10/26.
//

#include <iostream>
#include <engine/graphics/PostProcessingController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <glad/glad.h>

namespace engine::graphics {

void PostProcessingController::initialize() {
    auto platform = core::Controller::get<platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    //5
    glGenTextures(1, &m_color_texture);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //6
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

    //8
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);

    //9
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    //3
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        std::cerr <<"yey";
    }
    else std::cerr <<"no";
    //4
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    //
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);


    initialize_screen_quad();
    resize(width, height);
}

void PostProcessingController::begin_draw() {
    auto platform = core::Controller::get<platform::PlatformController>();
    resize(platform->window()->width(), platform->window()->height());
    begin_capture();
}

void PostProcessingController::present(const resources::Shader *shader) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,m_width, m_height);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    shader->set_int("screen_texture",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
    glEnable(GL_DEPTH_TEST);

}

void PostProcessingController::terminate() {destroy();}

void PostProcessingController::resize(int width, int height) {
    if(width <= 0 || height <= 0 || (width == m_width && height == m_height))
        return;
    m_width = width;
    m_height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0 ,GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);

    unsigned int fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    glBindTexture(GL_TEXTURE_2D,0);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    if(fb_status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr<< "grayscale error, status: " + fb_status;
}

void PostProcessingController::begin_capture() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0,0,m_width, m_height);
    glEnable(GL_DEPTH_TEST);
    OpenGL::clear_buffers();
}

void PostProcessingController::destroy() {
    if(m_quad_vbo != 0)
        glDeleteBuffers(1, &m_quad_vbo);
    if(m_quad_vao != 0)
        glDeleteVertexArrays(1, &m_quad_vao);
    if(m_rbo != 0)
        glDeleteRenderbuffers(1, &m_rbo);
    if(m_color_texture != 0)
        glDeleteTextures(1, &m_color_texture);
    if(m_framebuffer != 0)
        glDeleteFramebuffers(1, &m_framebuffer);

    m_quad_vbo = 0;
    m_quad_vao = 0;
    m_rbo = 0;
    m_color_texture = 0;
    m_framebuffer = 0;
    m_width = 0;
    m_height = 0;

}

void PostProcessingController::initialize_screen_quad() {
    const float quad_vertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);
    glBindVertexArray(m_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

} // engine