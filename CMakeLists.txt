cmake_minimum_required(VERSION 3.20..26)
project(elle)

set(CMAKE_CXX_STANDARD 23)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/glm/glm)

find_package(OpenGL REQUIRED)

include_directories(src)
include_directories(libs)
include_directories(libs/glm)

add_executable(elle src/main.cpp libs/stb/stb_truetype.h libs/stb/stb_image.h src/engine/vertex_array.h src/engine/uniform.h src/engine/usings.h src/engine/uniform.cpp src/engine/shader.h src/engine/shader.cpp src/engine/string_ext.h src/engine/app.h src/engine/event.h src/engine/event_args.h src/engine/utils.h src/engine/vertex_array.cpp src/engine/linked_vertex_array.cpp src/engine/linked_vertex_array.h src/engine/camera.cpp src/engine/camera.h libs/glad/glad.h libs/glad/glad.c src/engine/keybind.h src/engine/material.h src/engine/texture.cpp src/engine/texture.h libs/stb/stb_image_write.h src/engine/stb_image_def.cpp src/engine/logging.h src/engine/frame_buffer.h src/engine/frame_buffer.cpp src/engine/font.h src/engine/stb_truetype_def.cpp src/engine/font.cpp src/engine/layout.h src/engine/object.h src/engine/object.cpp src/engine/lines.h src/engine/lines.cpp src/engine/lazy.h src/engine/utils.cpp src/engine/joint.h src/engine/joint.cpp src/engine/pose.h src/engine/pose.cpp src/engine/layout.cpp src/elle/state.h src/elle/state_fn.h src/elle/editor.h src/elle/editor.cpp src/elle/state_fn.cpp)
find_package(assimp CONFIG REQUIRED)
find_package(cpptoml CONFIG REQUIRED)
target_link_libraries(elle PRIVATE glfw assimp::assimp cpptoml)