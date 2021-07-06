cmake_minimum_required(VERSION 3.10)

# cg_planet
project(cg_planet VERSION 1.0)

# C++ standard
set(CMAKE_CXX_STANDARD 20)

#cgmath library
add_subdirectory(cgmath)

#util library
add_subdirectory(util)

#wildcard additions sources
file(GLOB SOURCES "src/*.cc")

#imgui
set(IMGUI_SRC
    ${IMGUI_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

#Audio
add_subdirectory(audio)

# includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(cg_planet ${SOURCES} ${IMGUI_SRC})

#libraries
set (CMAKE_SHARED_LINKER_FLAGS "-DIMGUI_IMPL_OPENGL_LOADER_GLEW,`pkg-config --static --libs glfw3`,`pkg-config --cflags glfw3`")
set(LIBS cgmath util GL glfw GLEW m audio)

#threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(cg_planet ${LIBS} Threads::Threads)