cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(aieBootstrapProject)

#TODO: Update to use the target_complile_requires
set(CMAKE_CXX_STANDARD 14)
set(OpenGL_GL_PREFERENCE LEGACY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)


#add build glfw
option(GLFW_BUILD_EXAMPLES NO)
option(GLFW_BUILD_TESTS NO)
add_subdirectory(${PROJECT_SOURCE_DIR}/dependencies/glfw-source)

add_subdirectory(bootstrap)
add_subdirectory(mylib)


#PROJECT TO BUILD
add_subdirectory(physicsengine)
