#include <format>
#include <iostream>
#include <algorithm>
#include <string.h> // strcmp
#include <ranges>
#include "context.h"
#include "device.h"
#include "surface.h"
#include "swapchain.h"
#include "queue.h" //queueInfo
#include "utils.h"

using namespace std;

namespace SharedUtils
{
    VulkanContext::VulkanContext(
        std::shared_ptr<ILogicalDevice> device,
        std::shared_ptr<IRenderingSurface> surface,
        VkPresentModeKHR const &desiredPresentMode,
        const uint32_t desiredImageCount,
        std::set<VkImageUsageFlagBits> const &desiredImageUsage,
        VkSurfaceTransformFlagBitsKHR const &desiredTransform,
        VkSurfaceFormatKHR const &desiredFormat,
        const uint32_t desiredImageLayers) : _device(device),
                                             _surface(surface)
    {
        cout << format("--> VulkanContext::VulkanContext") << std::endl;
        _swapchain = std::make_unique<VulkanSwapChain>(device, surface, desiredPresentMode, desiredImageCount,
                                                       desiredImageUsage, desiredTransform, desiredFormat, desiredImageLayers);
        cout << format("<-- VulkanContext::VulkanContext") << std::endl;
    }

    VulkanContext::~VulkanContext()
    {
    }

    void VulkanContext::resize(const uint32_t width, const uint32_t height)
    {
        cout << format("--> VulkanContext::resize") << std::endl;
        auto pDevice = std::any_cast<VkPhysicalDevice>(_device->getPhysicalDevice());
        auto vkSurface = std::any_cast<VkSurfaceKHR>(_surface->getSurface());

        VkSurfaceCapabilitiesKHR surface_capabilities{};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, vkSurface, &surface_capabilities))
        // from wsi surface
        auto desiredImageExtent = surface_capabilities.currentExtent;
        _swapchain = std::make_unique<VulkanSwapChain>(*_swapchain);
        cout << format("<-- VulkanContext::resize") << std::endl;
    }
}
