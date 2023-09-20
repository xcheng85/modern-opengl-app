#include <format>
#include <iostream>
#include <algorithm>
#include "device.h"
#include "window.h"
#include "vulkan.h"
#include "utils.h"

using namespace std;

namespace SharedUtils
{
    VulkanPhysicalDevices::VulkanPhysicalDevices(std::shared_ptr<IRenderingContext> renderingContext)
    {
        cout << format("--> VulkanPhysicalDevices::VulkanPhysicalDevices") << std::endl;

        auto const &instance = renderingContext->getInstance();
        uint32_t physical_device_count{0};
        VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr));

        if (physical_device_count < 1)
        {
            throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
        }

        cout << format("<-- VulkanPhysicalDevices::VulkanPhysicalDevices") << std::endl;
    }

    VulkanPhysicalDevices::~VulkanPhysicalDevices()
    {
    }
}
