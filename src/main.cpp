#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "curves.hpp"
#include "curve_program.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// --- Configuration ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

curves::CurveProgram program;

// --- Shader Loading Utility ---
GLuint loadShaders(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        return 0;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << std::endl;
        return 0;
    }

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << std::endl;
        glDeleteShader(vertex); // Clean up vertex shader
        return 0;
    }

    // Shader Program
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

// --- Simple Orthographic Projection ---
// Creates a matrix to map coordinates from world space (-w/2 to w/2, -h/2 to h/2)
// to clip space (-1 to 1)
void createOrthoProjection(float* matrix, float left, float right, float bottom, float top, float nearVal, float farVal)
{
    matrix[0] = 2.0f / (right - left);
    matrix[4] = 0.0f;
    matrix[8] = 0.0f;
    matrix[12] = -(right + left) / (right - left);
    matrix[1] = 0.0f;
    matrix[5] = 2.0f / (top - bottom);
    matrix[9] = 0.0f;
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[2] = 0.0f;
    matrix[6] = 0.0f;
    matrix[10] = -2.0f / (farVal - nearVal);
    matrix[14] = -(farVal + nearVal) / (farVal - nearVal);
    matrix[3] = 0.0f;
    matrix[7] = 0.0f;
    matrix[11] = 0.0f;
    matrix[15] = 1.0f;
}

// Callback function for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Responsible for mouse clicks
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            program.press_mouse();
        }
        else if (action == GLFW_RELEASE)
        {
            program.release_mouse();
        }
    }
}

int main()
{
    // --- Initialize GLFW ---
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // --- Create Window ---
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OGLCurve", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- Initialize GLAD ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate(); // Terminate GLFW before returning
        return -1;
    }

    // Set initial viewport size
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // --- Load Shaders ---
    GLuint shaderProgram = loadShaders("shaders/line.vert", "shaders/line.frag");
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }
    
    program = curves::CurveProgram();

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // --- Setup Buffers (VAO, VBO) (This part is purely generated with ChatGPT) ---
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO); // Create Vertex Array Object
    glGenBuffers(1, &VBO);      // Create Vertex Buffer Object

    glBindVertexArray(VAO); // Bind VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind VBO to the GL_ARRAY_BUFFER target
    // Copy the point data into the VBO
    glBufferData(GL_ARRAY_BUFFER, program.get_line_coords().size() * sizeof(float), program.get_line_coords().data(), GL_STATIC_DRAW);

    // Configure vertex attributes (tell OpenGL how to interpret the VBO data)
    // layout (location = 0) in vec2 aPos; -> location 0
    // vec2 -> size 2
    // float -> type GL_FLOAT
    // Normalize? -> GL_FALSE
    // Stride (bytes between vertices) -> 2 * sizeof(float)
    // Offset (bytes from start) -> 0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable the vertex attribute (location 0)

    // Unbind VBO and VAO (good practice, prevents accidental modification)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- Projection Matrix (also ChatGPT) ---
    // Use orthographic projection for 2D. Map world coordinates (e.g., -1 to 1)
    // to OpenGL's normalized device coordinates (-1 to 1).
    float projection[16];
    // Simple projection matching common clip space for now
    createOrthoProjection(projection, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    // If you want coordinates to map directly to pixels (0,0 top-left):
    // createOrthoProjection(projection, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
    // Adjust your 'points' vector accordingly if you change the projection.

    // --- Render Loop ---
    while (!glfwWindowShouldClose(window))
    {
        // --- Input ---
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        program.update_drag(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT);         // Clear framebuffer

        // Use the shader program
        glUseProgram(shaderProgram);

        // Set the projection uniform in the vertex shader
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        // Bind the VAO (which contains the VBO configuration)
        glBindVertexArray(VAO);

        // Update the points for the line and crosses
        program.refresh_line();
        // TODO check if I really need this
        glfwSetMouseButtonCallback(window, mouse_button_callback);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, program.get_line_coords().size() * sizeof(float), program.get_line_coords().data());

        // Number of vertices to draw
        int numVertices = program.get_line_coords().size() / 2;
        // Draw the lines!
        // GL_LINE_STRIP connects vertices in a line
        glDrawArrays(GL_LINE_STRIP, 0, numVertices - 64);
        // GL_LINES draws the crosses (like cross-strokes)
        glDrawArrays(GL_LINES, numVertices - 64, 64);

        // Unbind VAO
        glBindVertexArray(0);

        // --- Swap Buffers and Poll Events ---
        glfwSwapBuffers(window); // Show the rendered frame
        glfwPollEvents();        // Check for input events (keyboard, mouse, window close)
    }

    // --- 9. Cleanup ---
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate(); // Clean up GLFW resources
    return 0;
}
