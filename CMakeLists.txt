cmake_minimum_required(VERSION 3.20)
project(openGLTwo)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
CMAKE_POLICY(SET CMP0111 NEW)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glad REQUIRED)
find_package(imgui REQUIRED)
find_package(enet REQUIRED)

add_executable(enetHost src/enetHost.cpp)
add_executable(openGLTwo
src/includes.h
src/globals.h
src/indexBuffer.cpp
src/main.cpp
src/object.cpp 
src/renderer.cpp 
src/vertexArray.cpp 
src/vertexBuffer.cpp
src/camera.h 
bindings/imgui_impl_glfw.cpp
bindings/imgui_impl_glfw.h
bindings/imgui_impl_opengl3.cpp
bindings/imgui_impl_opengl3.h
bindings/imgui_impl_opengl3_loader.h
bindings/imgui.cpp
bindings/imgui_draw.cpp
bindings/imgui_demo.cpp
bindings/imgui_tables.cpp
bindings/imgui_widgets.cpp
)




target_link_libraries(openGLTwo PRIVATE
glm::glm
opengl::opengl
glfw
imgui::imgui
enet::enet
)
target_link_libraries(enetHost PRIVATE enet::enet)
target_link_libraries(openGLTwo PUBLIC glad::glad)
