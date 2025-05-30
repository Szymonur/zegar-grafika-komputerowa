cmake_minimum_required(VERSION 3.16)
project(MechanicalClock)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)


# Dodaj ścieżki do Homebrew
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_executable(MechanicalClock
    src/main.cpp
    src/glad/include/glad.c
)

target_include_directories(MechanicalClock PRIVATE src/glad/include)

target_link_libraries(MechanicalClock
    glfw
    "-framework OpenGL"
)