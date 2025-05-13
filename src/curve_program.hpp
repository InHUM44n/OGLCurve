#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "curves.hpp"

#include <string>

namespace curves
{
    class CurveProgram
    {
    public:
        CurveProgram();
        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void refresh_line();
        void press_mouse();
        void release_mouse();
        void update_drag(GLFWwindow* window);
        const std::vector<float>& get_line_coords() const;
    private:
        // Variables to change the points later
        curves::CurveType type;
        int clicks;
        std::vector<float> line_coords;
        std::vector<std::vector<float>> points;
        bool mouseHeld;
    };
}