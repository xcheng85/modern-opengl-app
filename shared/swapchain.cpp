#include <format>
#include <iostream>
#include <algorithm>
#include <ranges>
#include "swapchain.h"
#include "device.h"
#include "surface.h"
#include "queue.h"
#include "utils.h"

using namespace std;

namespace SharedUtils
{
    VulkanSwapChain::VulkanSwapChain(
        std::shared_ptr<ILogicalDevice> device,
        std::shared_ptr<IRenderingSurface> surface,
        VkPresentModeKHR const &desiredPresentMode,
        const uint32_t desiredImageCount,
        VkExtent2D const &desiredImageExtent,
        std::set<VkImageUsageFlagBits> const &usage,
        VkSurfaceTransformFlagBitsKHR const &transform,
        VkSurfaceFormatKHR const &desiredFormat,
        const uint32_t desiredImageLayers)
    {
        cout << std::format("--> VulkanSwapChain::VulkanSwapChain") << std::endl;

        auto pDevice = std::any_cast<VkPhysicalDevice>(device->getPhysicalDevice());
        auto vkSurface = std::any_cast<VkSurfaceKHR>(surface->getSurface());

        // present mode
        uint32_t present_mode_count{0U};
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, vkSurface, &present_mode_count, nullptr));
        std::vector<VkPresentModeKHR> present_modes(present_mode_count, VkPresentModeKHR{});
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, vkSurface, &present_mode_count, present_modes.data()));
        cout << std::format("Surface supports the following present mode:");
        for (auto &present_mode : present_modes)
        {
            cout << std::format("{}", std::to_string(present_mode)) << std::endl;
        }

        // cap
        VkSurfaceCapabilitiesKHR surface_capabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, vkSurface, &surface_capabilities);

        // format
        uint32_t surface_format_count{0U};
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, vkSurface, &surface_format_count, nullptr));
        std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count, VkSurfaceFormatKHR{});
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, vkSurface, &surface_format_count, surface_formats.data()));

        cout << std::format("Surface supports the following surface formats:");
        for (auto &surface_format : surface_formats)
        {
            cout << std::format("{}", surface_format) << std::endl;
        }

        VkSwapchainCreateInfoKHR create_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
        // lamda with reference capture
        auto clampImageCount = [&surface_capabilities](uint32_t imageCount)
        {
            auto min_image_count = surface_capabilities.minImageCount;
            auto max_image_count = surface_capabilities.maxImageCount;
            return std::max(min_image_count, std::min(imageCount, max_image_count));
        };
        create_info.minImageCount = clampImageCount(desiredImageCount);

        auto clampImageExtent = [&surface_capabilities](VkExtent2D const &desiredImageExtent)
        {
            auto [minWidth, minHeight] = surface_capabilities.minImageExtent;
            auto [maxWidth, maxHeight] = surface_capabilities.maxImageExtent;
            auto const &[w, h] = desiredImageExtent;
            return VkExtent2D{
                std::max(minWidth, std::min(w, maxWidth)),
                std::max(minHeight, std::min(h, maxHeight))};
        };
        create_info.imageExtent = clampImageExtent(desiredImageExtent);

        create_info.presentMode = desiredPresentMode;
        // lamda inside lamda
        auto pickImageFormat = [&surface_formats](VkSurfaceFormatKHR const &desiredFormat)
        {
            auto surface_format_it = std::find_if(
                surface_formats.begin(),
                surface_formats.end(),
                [&desiredFormat](const VkSurfaceFormatKHR &availabeSurfaceFormat)
                {
                    if (availabeSurfaceFormat.format == desiredFormat.format &&
                        availabeSurfaceFormat.colorSpace == desiredFormat.colorSpace)
                    {
                        return true;
                    }
                    return false;
                });

            if (surface_format_it == surface_formats.end())
            {
                // fall back to the first one
                surface_format_it = surface_formats.begin();
            }
            return *surface_format_it;
        };
        auto [format, colorSpace] = pickImageFormat(desiredFormat);
        create_info.imageFormat = format;
        create_info.imageColorSpace = colorSpace;

        auto clampImageLayers = [&surface_capabilities](uint32_t desiredImageLayers)
        {
            auto max_image_layers = surface_capabilities.maxImageArrayLayers;
            return std::max(1u, std::min(desiredImageLayers, max_image_layers));
        };
        create_info.imageArrayLayers = clampImageLayers(desiredImageLayers);
        // image usage
        auto pickImageUsasge = [&surface_formats](VkSurfaceFormatKHR const &desiredFormat)
        {
            auto surface_format_it = std::find_if(
                surface_formats.begin(),
                surface_formats.end(),
                [&desiredFormat](const VkSurfaceFormatKHR &availabeSurfaceFormat)
                {
                    if (availabeSurfaceFormat.format == desiredFormat.format &&
                        availabeSurfaceFormat.colorSpace == desiredFormat.colorSpace)
                    {
                        return true;
                    }
                    return false;
                });

            if (surface_format_it == surface_formats.end())
            {
                // fall back to the first one
                surface_format_it = surface_formats.begin();
            }
            return *surface_format_it;
        };

        create_info.imageUsage = properties.image_usage;
        // create_info.preTransform = properties.pre_transform;
        // create_info.compositeAlpha = properties.composite_alpha;
        // create_info.oldSwapchain = properties.old_swapchain;
        // create_info.surface = surface;

        cout << std::format("<-- VulkanSwapChain::VulkanSwapChain") << std::endl;
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
    }
}
