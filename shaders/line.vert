#version 330 core
layout (location = 0) in vec2 aPos; // Input vertex position (x, y)

// Uniform for projection matrix (to handle different window aspect ratios)
uniform mat4 projection;

void main()
{
    // Output position in clip space. z=0 for 2D, w=1.
    gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);
}