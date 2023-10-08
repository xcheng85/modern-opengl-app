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
        const uint32_t desiredImageLayers,
        VulkanSwapChain *old)
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
        this->_imageExtent = desiredImageExtent;

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
        if (!old)
        {
            create_info.oldSwapchain = VK_NULL_HANDLE;
        }
        else
        {
            auto old_swapchain = any_cast<VkSwapchainKHR>(old->getSwapChain());
            create_info.oldSwapchain = old_swapchain;
        }
        create_info.surface = vkSurface;

        auto vkDevice = std::any_cast<VkDevice>(device->getDevice());
        VK_CHECK(vkCreateSwapchainKHR(vkDevice, &create_info, nullptr, &_swapchain));

        uint32_t numImagesInSwapChain{0u};
        VK_CHECK(vkGetSwapchainImagesKHR(vkDevice, _swapchain, &numImagesInSwapChain, nullptr));
        _images.resize(numImagesInSwapChain);
        VK_CHECK(vkGetSwapchainImagesKHR(vkDevice, _swapchain, &numImagesInSwapChain, _images.data()));

        // create image views
        for (size_t i = 0; i < _images.size(); i++)
        {
            // Create an image view which we can render into.
            VkImageViewCreateInfo view_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = format;
            view_info.image = _images[i];
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.layerCount = 1;
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.components.r = VK_COMPONENT_SWIZZLE_R;
            view_info.components.g = VK_COMPONENT_SWIZZLE_G;
            view_info.components.b = VK_COMPONENT_SWIZZLE_B;
            view_info.components.a = VK_COMPONENT_SWIZZLE_A;

            VkImageView image_view;
            VK_CHECK(vkCreateImageView(vkDevice, &view_info, nullptr, &image_view));

            _image_views.push_back(image_view);
        }

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
                          current._imageUsageBits, current._transform, current._surfaceFormat, current._imageLayers,
                          &current)
    {
        cout << std::format("--> VulkanSwapChain::VulkanSwapChain(VulkanSwapChain &current)") << std::endl;
        cout << std::format("<-- VulkanSwapChain::VulkanSwapChain(VulkanSwapChain &current)") << std::endl;
    }

    VulkanSwapChain::VulkanSwapChain(VulkanSwapChain &&other) noexcept
    {
        cout << std::format("--> VulkanSwapChain::VulkanSwapChain(VulkanSwapChain && other)") << std::endl;
        _swapchain = other._swapchain;
        other._swapchain = VK_NULL_HANDLE;
        cout << std::format("<-- VulkanSwapChain::VulkanSwapChain(VulkanSwapChain && other)") << std::endl;
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        if (_swapchain != VK_NULL_HANDLE)
        {
            auto vkDevice = std::any_cast<VkDevice>(_device->getDevice());
            for (VkImageView image_view : _image_views)
            {
                vkDestroyImageView(vkDevice, image_view, nullptr);
            }

            vkDestroySwapchainKHR(vkDevice, _swapchain, nullptr);
        }
    }

    VkResult VulkanSwapChain::acquireNextImage(uint32_t &image_index, VkSemaphore image_acquired_semaphore, VkFence fence) const
    {
        auto vkDevice = std::any_cast<VkDevice>(_device->getDevice());
        return vkAcquireNextImageKHR(vkDevice, _swapchain, std::numeric_limits<uint64_t>::max(), image_acquired_semaphore, fence, &image_index);
    }
}
