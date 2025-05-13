#include "curve_program.hpp"

namespace curves
{
    CurveProgram::CurveProgram()
    {
        // --- Default Points for the cubic curve ---
        // coordinates centered around (0,0).
        // Each pair is an (x, y) point.
        clicks = 0;
        // default points (Cubic Bezier)
        type = curves::CurveType::CubicBezier;
        points = { { -0.8f, -0.5f }, { -0.4f, 0.5f }, { 0.0f, -0.5f }, { 0.4f, 0.5f } };
        refresh_line();
    }
    
    void CurveProgram::press_mouse()
    {
        mouseHeld = true;
    }
    
    void CurveProgram::release_mouse()
    {
        mouseHeld = false;
#ifdef DEBUG// print coordinates for current point on screen
        std::vector<float> p = points.at(clicks);
        std::string log_info = "P" + std::to_string(clicks) + ": " + std::to_string(p[0]) + " " + std::to_string(p[1]) + "\n";
        std::fprintf(stdout, log_info.c_str());
#endif
        // increment clicks to edit the next Point
        switch (type)
        {
        case CurveType::CubicBezier:
            clicks = (clicks + 1) % 4;
            break;
        case CurveType::Lagrange:
            clicks++;
            break;
        }
    }
    
    void CurveProgram::update_drag(GLFWwindow* window)
    {
        if (!mouseHeld || clicks >= points.size()) return;
        // store x and y position of mouse cursor
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // store x and y size of window
        int xwindow, ywindow;
        glfwGetWindowSize(window, &xwindow, &ywindow);
        // adjust position for the projection [-1 : 1]
        xpos = ((xpos / xwindow) - 0.5) * 2;
        ypos = -((ypos / ywindow) - 0.5) * 2;
        // override the values of the Point vector
        points[clicks][0] = xpos;
        points[clicks][1] = ypos;
    }
    
    const std::vector<float>& CurveProgram::get_line_coords() const {
        return line_coords;
    }
    
    void CurveProgram::refresh_line()
    {
        switch (type)
        {
        case curves::CurveType::CubicBezier:
            line_coords = curves::genCubicBezierCurve(100, points.at(0), points.at(1), points.at(2), points.at(3));
            break;
        case curves::CurveType::Lagrange:
            line_coords = curves::genLagrangeCurve(100, points);
            break;
        }
        // add the point markers
        for (float cross_coordinate : curves::genCrosses(points))
            line_coords.push_back(cross_coordinate);
    }
}