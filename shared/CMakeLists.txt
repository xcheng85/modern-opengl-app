cmake_minimum_required(VERSION 3.27)

project(SharedUtils CXX C)

include(../cmake/SharedMacros.txt)

include_directories(../external/src/vulkan/include)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c??)
file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

add_library(SharedUtils ${SRC_FILES} ${HEADER_FILES})

set_property(TARGET SharedUtils PROPERTY CXX_STANDARD 20)
set_property(TARGET SharedUtils PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(SharedUtils PUBLIC glad glfw volk glslang SPIRV )


# target_link_libraries(target_name 
#    PUBLIC 
#      glslang::glslang 
#      glslang::SPIRV 
#      glslang::glslang-default-resource-limits)