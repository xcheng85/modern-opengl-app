cmake_minimum_required(VERSION 3.27)

project(Vulkan_ioc)

include(../../cmake/SharedMacros.txt)

SETUP_APP(vulkan_ioc "vulkan")

include_directories(../../external/src/vulkan/include)
include_directories(../../shared)

target_link_libraries(vulkan_ioc PUBLIC glfw volk SharedUtils)

# without volk
# target_link_libraries(shader-compilation PUBLIC glfw Vulkan::Vulkan)