#define VK_NO_PROTOTYPES // volk
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define VOLK_IMPLEMENTATION
// #include "volk.h"

// following header include volk.sh and glslang_c_interface.h
#include "shared/vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "shared/di.hpp"
namespace di = boost::di;

using namespace SharedUtils;

int main()
{
    glslang_initialize_process();

    volkInitialize();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";
    std::vector<std::string> v{"VK_LAYER_KHRONOS_validation"};
    std::vector<std::string> e
    {
        "VK_KHR_surface",
#if defined(_WIN32)
            "VK_KHR_win32_surface"
#endif
#if defined(__APPLE__)
            "VK_MVK_macos_surface"
#endif
#if defined(__linux__)
            "VK_KHR_xcb_surface"
#endif
            ,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };
    auto injector = di::make_injector(
        di::bind<IRenderingContextValidationLayer>().to<VulkanRenderingContextValidationLayers>(),
        di::bind<IRenderingContextExtensions>().to<VulkanRenderingContextExtensions>(),
        di::bind<IRenderingHostAppSettings>().to<VulkanRenderingHostAppSettings>(),

        di::bind<std::string[]>().named(VALIDATION_LAYERS).to(v),
        di::bind<std::string[]>().named(VULKAN_EXTENSIONS).to(e),
        di::bind<std::string>().named(APP_NAME).to("VULKAN_IOC"),
        di::bind<std::string>().named(APP_VERSION).to("0.0.1"));

    injector.create<VulkanRenderingContext>();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    glslang_finalize_process();

    return 0;
}