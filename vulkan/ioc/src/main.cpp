#define VK_NO_PROTOTYPES // volk
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define VOLK_IMPLEMENTATION
// #include "volk.h"

// following header include volk.sh and glslang_c_interface.h
#include "shared/vulkan.h"
#include "shared/platform.h"
#include "shared/window.h"
#include "shared/application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "shared/di.hpp"
namespace di = boost::di;

using namespace SharedUtils;
using namespace std;

class IocApplication : public Application
{
public:
    IocApplication() : Application()
    {
        cout << "Ioc ctor" << std::endl;
    };
};

int main()
{
    auto injector = di::make_injector(
        di::bind<IWindow>().to<GlfwWindow>(),
        di::bind<IApplication>().to<IocApplication>());

    auto platform = injector.create<std::unique_ptr<UnixPlatform>>();
    platform.get()->main_loop();
    return 0;
    /*
    glslang_initialize_process();

    volkInitialize();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    // uint32_t extensionCount = 0;
    // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    // std::cout << extensionCount << " extensions supported\n";

    auto injector = di::make_injector(
        di::bind<IRenderingContextValidationLayer>().to<VulkanRenderingContextValidationLayers>(),
        di::bind<IRenderingContextExtensions>().to<VulkanRenderingContextExtensions>(),
        di::bind<IRenderingHostAppSettings>().to<VulkanRenderingHostAppSettings>(),

        // di::bind<ConfigString[]>().named(VALIDATION_LAYERS).to({ConfigString{"VK_LAYER_KHRONOS_validation"}}),
        // di::bind<ConfigString[]>().named(VULKAN_EXTENSIONS).to({ConfigString{"VK_KHR_surface"}, ConfigString{
        //                                                                                             "VK_KHR_surface"
        //                                                                                         },
        //                                                         ConfigString{"VK_KHR_surface"}, ConfigString{"VK_KHR_surface"}}),
        di::bind<std::string>().named(APP_NAME).to("VULKAN_IOC"), di::bind<std::string>().named(APP_VERSION).to("0.0.1"));

    injector.create<VulkanRenderingDebugger>();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    glslang_finalize_process();

    return 0;*/
}