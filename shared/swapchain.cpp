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
        std::set<VkImageUsageFlagBits> const &desiredImageUsage,
        VkSurfaceTransformFlagBitsKHR const &desiredTransform,
        VkSurfaceFormatKHR const &desiredFormat,
        const uint32_t desiredImageLayers)
        : _device(device),
          _surface(surface),
          _presentMode(desiredPresentMode),
          _imageCount(desiredImageCount),
          _imageUsageBits(desiredImageUsage),
          _transform(desiredTransform),
          _surfaceFormat(desiredFormat),
          _imageLayers(desiredImageLayers)
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
        auto desiredImageExtent = surface_capabilities.currentExtent;

        // format
        uint32_t surface_format_count{0U};
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, vkSurface, &surface_format_count, nullptr));
        std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count, VkSurfaceFormatKHR{});
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, vkSurface, &surface_format_count, surface_formats.data()));

        cout << std::format("Surface supports the following surface formats:");
        for (auto &surface_format : surface_formats)
        {
            cout << std::format("format: {}, colorSpace: {}", to_string(surface_format.format), to_string(surface_format.colorSpace)) << std::endl;
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
        auto pickSurfaceFormat = [&surface_formats](VkSurfaceFormatKHR const &desiredFormat)
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
        // auto [format, colorSpace] = pickImageFormat(desiredFormat);
        auto surfaceFormat = pickSurfaceFormat(desiredFormat);
        auto [format, colorSpace] = surfaceFormat;
        create_info.imageFormat = format;
        create_info.imageColorSpace = colorSpace;

        auto clampImageLayers = [&surface_capabilities](uint32_t desiredImageLayers)
        {
            auto max_image_layers = surface_capabilities.maxImageArrayLayers;
            return std::max(1u, std::min(desiredImageLayers, max_image_layers));
        };
        create_info.imageArrayLayers = clampImageLayers(desiredImageLayers);
        // image usage
        // desired: set of VkImageUsageFlagBits
        // supported usage bits: from the physical device + surface
        // format_properties.optimalTilingFeatures: all the image usage that optimal Tiling supported.
        // format_properties.linearTilingFeatures: all the image usage that linear Tiling supported.
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(pDevice, format, &format_properties);
        auto validateImageUsage = [&surface_capabilities, &format_properties](std::set<VkImageUsageFlagBits> const &desiredImageUsage)
        {
            auto supportedImageUsage = surface_capabilities.supportedUsageFlags;
            auto supportedFeaturesForFormat = format_properties.optimalTilingFeatures;
            // all the image usage that support optimal tiling
            // lamda with trailing return type: more like typescript
            auto validateImageFormatFeatures = [](VkImageUsageFlagBits const &desiredImageUsageFlagBit, VkFormatFeatureFlags const &supportedFeaturesForFormat) -> bool
            {
                switch (desiredImageUsageFlagBit)
                {
                case VK_IMAGE_USAGE_STORAGE_BIT:
                    return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supportedFeaturesForFormat;
                default:
                    return true;
                }
            };

            for (auto imageUsageFlagBit : desiredImageUsage)
            {
                if ((imageUsageFlagBit & supportedImageUsage) && validateImageFormatFeatures(imageUsageFlagBit, supportedFeaturesForFormat))
                {
                }
                else
                {
                    throw std::runtime_error(std::format("desired image usage bit {} is not supported", to_string(imageUsageFlagBit)));
                }
            }
        };
        auto compositeToImageUsageFlags = [](std::set<VkImageUsageFlagBits> const &desiredImageUsage) -> VkImageUsageFlags
        {
            VkImageUsageFlags image_usage{};
            for (auto flag : desiredImageUsage)
            {
                image_usage |= flag;
            }
            return image_usage;
        };
        validateImageUsage(desiredImageUsage);
        create_info.imageUsage = compositeToImageUsageFlags(desiredImageUsage);

        // transform of image before present
        auto validateTransform = [&surface_capabilities](VkSurfaceTransformFlagBitsKHR const &desiredTransform)
        {
            auto supportedTransforms = surface_capabilities.supportedTransforms;
            if (desiredTransform & supportedTransforms)
            {
            }
            else
            {
                throw std::runtime_error(std::format("desired surface transform {} is not supported", to_string(desiredTransform)));
            }
        };

        create_info.preTransform = desiredTransform;
        // Alpha blending with swapchain surface with the rest of window
        // VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR: disable alpha blending
        // VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        // VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        // VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR};
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.oldSwapchain = VK_NULL_HANDLE;
        create_info.surface = vkSurface;

        auto vkDevice = std::any_cast<VkDevice>(device->getDevice());
        VK_CHECK(vkCreateSwapchainKHR(vkDevice, &create_info, nullptr, &_swapchain));
        cout << std::format("<-- VulkanSwapChain::VulkanSwapChain") << std::endl;
    }

    VulkanSwapChain::VulkanSwapChain(ISwapChain &current)
        : VulkanSwapChain(dynamic_cast<VulkanSwapChain &>(current))
    {
        cout << std::format("--> VulkanSwapChain::VulkanSwapChain(ISwapChain &current)") << std::endl;
        cout << std::format("<-- VulkanSwapChain::VulkanSwapChain(ISwapChain &current)") << std::endl;
    }

    VulkanSwapChain::VulkanSwapChain(VulkanSwapChain &current)
        : VulkanSwapChain(current._device, current._surface, current._presentMode, current._imageCount,
                          current._imageUsageBits, current._transform, current._surfaceFormat, current._imageLayers)
    {
        cout << std::format("--> VulkanSwapChain::VulkanSwapChain(VulkanSwapChain &current)") << std::endl;
        cout << std::format("<-- VulkanSwapChain::VulkanSwapChain(VulkanSwapChain &current)") << std::endl;
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        if (_swapchain != VK_NULL_HANDLE)
        {
            auto vkDevice = std::any_cast<VkDevice>(_device->getDevice());
            vkDestroySwapchainKHR(vkDevice, _swapchain, nullptr);
        }
    }
}
