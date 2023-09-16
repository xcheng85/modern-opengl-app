cmake_minimum_required(VERSION 3.27)

project(modern-opengl-app CXX C)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(external/src/glfw)
add_subdirectory(external/src/glslang)
add_subdirectory(external/src/volk)

include_directories(.)
include_directories(external/src/glfw/include)
include_directories(external/src/glm)
include_directories(external/src/gli)
include_directories(external/glad/include)
include_directories(external/src/glslang/glslang/Include)
include_directories(external/src/glslang)
include_directories(external/src/glslang/StandAlone)
include_directories(external/src/glslang/src)

set(GLAD_SOURCES external/glad/src/gl.c external/glad/src/vulkan.c)

add_library(glad ${GLAD_SOURCES})

set_property(TARGET glfw  PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glad  PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET volk  PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glslang PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET SPIRV PROPERTY FOLDER "ThirdPartyLibraries/glslang")

# shared utils code for GL and Vulkan
add_subdirectory(shared)
target_sources(SharedUtils PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/src/glslang/StandAlone/ResourceLimits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/src/glslang/StandAlone/resource_limits_c.cpp)

add_subdirectory(opengl/hello-world)
add_subdirectory(vulkan/shader-compilation)
add_subdirectory(vulkan/ioc)