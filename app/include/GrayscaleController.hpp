//
// Created by matfrg on 9/10/26.
//

#ifndef GRAYSCALECONROLLER_HPP
#define GRAYSCALECONROLLER_HPP
#include <engine/graphics/PostProcessingController.hpp>

namespace app {

class GrayscaleController : public engine::graphics::PostProcessingController {
public:
    std::string_view name() const override { return "app::GrayscaleController"; }

private:
    void poll_events() override;

    void draw() override;

    bool m_grayscale_enabled = false;
};

}// namespace app

#endif//GRAYSCALECONROLLER_HPP