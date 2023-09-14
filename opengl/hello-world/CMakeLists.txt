cmake_minimum_required(VERSION 3.27)

project(GL_HelloWorld)

include(../../cmake/SharedMacros.txt)

SETUP_APP(hello-world "opengl")

target_link_libraries(hello-world glfw)
