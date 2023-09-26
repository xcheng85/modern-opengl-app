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
#include "shared/surface.h"
#include "shared/device.h"
#include "shared/queue.h"

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
    IocApplication(
        std::shared_ptr<IRenderingDebugger>,
        std::shared_ptr<IRenderingSurface>,
        std::shared_ptr<IPhysicalDeviceList>,
        std::shared_ptr<ILogicalDevice>,
        std::shared_ptr<IDeviceQueueList>) : Application()
    {
        cout << "Ioc ctor" << std::endl;
    };
};

int main()
{
    glslang_initialize_process();

    volkInitialize();

    auto framework_module = []
    {
        return di::make_injector(
            di::bind<IWindow>().to<GlfwWindow>(),
            di::bind<IApplication>().to<IocApplication>());
    };

    std::initializer_list<std::string> validationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    std::initializer_list<std::string> instanceExts = {
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};

    std::initializer_list<std::string> desiredDeviceExts = {
        "VK_KHR_get_memory_requirements2",
        "VK_KHR_dedicated_allocation",
        "VK_KHR_get_memory_requirements2",
        "VK_KHR_dedicated_allocation",
        // "VK_KHR_performance_query", // my gpu not support it
        "VK_EXT_host_query_reset",
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};
    // lamda + copy capture
    auto vulkan_module = [validationLayers, instanceExts, desiredDeviceExts]
    {
        return di::make_injector(
            di::bind<string[]>.named(VALIDATION_LAYERS).to(validationLayers),
            di::bind<string[]>.named(INSTANCE_EXTENSIONS).to(instanceExts),
            di::bind<IRenderingHostAppSettings>().to<VulkanRenderingHostAppSettings>(),
            di::bind<IRenderingContext>().to<VulkanRenderingContext>(),
            di::bind<IRenderingSurface>().to<VulkanRenderingSurface>(),
            di::bind<IPhysicalDeviceList>().to<VulkanPhysicalDeviceList>(),
            di::bind<ILogicalDevice>().to<VulkanLogicalDevice>(),
            di::bind<string[]>.named(DESIRE_PHYSICAL_DEVICE_EXTS).to(desiredDeviceExts),
            di::bind<VkPhysicalDeviceFeatures>().named(DESIRE_PHYSICAL_DEVICE_FEATURES).to(VkPhysicalDeviceFeatures{}),
            di::bind<VkQueueFlagBits>().named(DESIRE_QUEUE_FAMILY_CAPABILITY).to(VK_QUEUE_GRAPHICS_BIT),
            di::bind<IDeviceQueueList>().to<VulkanLogicDeviceQueueList>(),
            di::bind<std::string>().named(APP_NAME).to("VULKAN_IOC"), di::bind<std::string>().named(APP_VERSION).to("0.0.1"));
    };

    auto injector = di::make_injector(framework_module(), vulkan_module());
    auto platform = injector.create<std::unique_ptr<UnixPlatform>>();
    platform.get()->main_loop();

    glslang_finalize_process();
    return 0;
}