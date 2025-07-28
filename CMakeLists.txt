cmake_minimum_required(VERSION 3.10)
project(IgnitionEngine)

set(CMAKE_CXX_STANDARD 17)

# Include GLFW
add_subdirectory(libs/glfw)

# Include directories
include_directories(
    src
    libs/glfw/include
    libs/glad/include
    libs/imgui
    libs/imgui/backends
    libs/json
    libs/tinyfiledialogs
)

# Source files
set(ENGINE_SOURCES
    libs/tinyfiledialogs/tinyfiledialogs.c
    src/main.cpp
    src/editor/ViewportPanel.cpp
    src/editor/Library.cpp
    src/editor/HierarchyPanel.cpp
    src/editor/InspectorPanel.cpp
    src/object/Scene.cpp
    src/object/Transform.cpp
    src/object/GameObject.cpp
)

set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)

set(GLAD_SOURCE libs/glad/src/glad.c)

add_executable(IgnitionEngine
    ${ENGINE_SOURCES}
    ${IMGUI_SOURCES}
    ${GLAD_SOURCE}
)

target_link_libraries(IgnitionEngine glfw ${CMAKE_DL_LIBS})
target_link_libraries(IgnitionEngine opengl32)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)
add_compile_definitions(_GLFW_WIN32)
