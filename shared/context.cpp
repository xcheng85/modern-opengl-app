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
        std::unique_ptr<ISwapChain> swapchain) : _swapchain{std::move(swapchain)},
                                                 _device(device),
                                                 _surface(surface)
    {
        cout << format("--> VulkanContext::VulkanContext") << std::endl;

        cout << format("<-- VulkanContext::VulkanContext") << std::endl;
    }

    VulkanContext::~VulkanContext()
    {
    }

    void VulkanContext::resize(const uint32_t width, const uint32_t height)
    {
        auto pDevice = std::any_cast<VkPhysicalDevice>(_device->getPhysicalDevice());
        auto vkSurface = std::any_cast<VkSurfaceKHR>(_surface->getSurface());

        VkSurfaceCapabilitiesKHR surface_capabilities{};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, vkSurface, &surface_capabilities))
        // from wsi surface
        auto desiredImageExtent = surface_capabilities.currentExtent;

        _swapchain = std::make_unique<VulkanSwapChain>(*_swapchain);
    }
}
