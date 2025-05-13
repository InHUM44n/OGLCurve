cmake_minimum_required(VERSION 3.10)
project(OpenGLLineExample C CXX) # Add C for glad.c

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Find GLFW ---
cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(glfw3 3.3 REQUIRED)

# --- Add GLAD ---
# Assuming glad source and includes are in src/ and include/ relative to CMakeLists.txt
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)

# --- Add Executable ---
add_executable(
    opengl_line_app
    src/main.cpp
    src/curve_program.cpp
    src/curves.cpp
)

# Add -DDEBUG only in Debug mode
target_compile_definitions(opengl_line_app PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# --- Link Libraries ---
target_link_libraries(opengl_line_app PRIVATE glfw glad) # Link GLFW and GLAD

# Add include directories for our project and glad
target_include_directories(opengl_line_app PRIVATE include)

# --- Platform Specific OpenGL Libraries (CMake usually handles this with find_package(OpenGL)) ---
find_package(OpenGL REQUIRED)
target_link_libraries(opengl_line_app PRIVATE OpenGL::GL)

# --- Copy Shaders to Build Directory (Optional, for convenience) ---
# Adjust path if needed
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})