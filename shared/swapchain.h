#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <volk.h>
#include "di.hpp"
#include "utils.h"

// physical device
// surface from wsi
// present mode
// surface cap
// number of images
// w,h of image: fetch from wsi surface
// usage
// surface transform
// format: rgba, precision, encoding color linear / non-linear space
// logic device
// acquire images
// give back images

namespace SharedUtils
{
    class ISwapChain
    {
    public:
        virtual ~ISwapChain() noexcept = default;
        virtual std::any getSwapChain() = 0;
        // explicit ISwapChain(ISwapChain &&) = delete;
    };

    class ILogicalDevice;
    class IRenderingSurface;

    class VulkanSwapChain : public ISwapChain
    {
    public:
        explicit VulkanSwapChain() = delete;
        // BOOST_DI_INJECT(VulkanSwapChain,
        //                 std::shared_ptr<ILogicalDevice>,
        //                 std::shared_ptr<IRenderingSurface>,
        //                 (named = DESIRE_SWAPCHAIN_PRESENT_MODE) VkPresentModeKHR const &,
        //                 (named = DESIRE_SWAPCHAIN_IMAGES_COUNT) const uint32_t,
        //                 (named = DESIRE_SWAPCHAIN_IMAGES_USAGE) std::set<VkImageUsageFlagBits> const &,
        //                 (named = DESIRE_SWAPCHAIN_IMAGES_TRANSFORMATION) VkSurfaceTransformFlagBitsKHR const &,
        //                 (named = DESIRE_SWAPCHAIN_IMAGES_FORMAT) VkSurfaceFormatKHR const &,
        //                 (named = DESIRE_SWAPCHAIN_IMAGES_LAYERS) const uint32_t);
        explicit VulkanSwapChain(
            std::shared_ptr<ILogicalDevice>,
            std::shared_ptr<IRenderingSurface>,
            VkPresentModeKHR const &,
            const uint32_t,
            std::set<VkImageUsageFlagBits> const &,
            VkSurfaceTransformFlagBitsKHR const &,
            VkSurfaceFormatKHR const &,
            const uint32_t,
            VkSwapchainKHR old = VK_NULL_HANDLE
        );
        // resize
        // ctor overload
        explicit VulkanSwapChain(ISwapChain &);
        explicit VulkanSwapChain(VulkanSwapChain &);
        explicit VulkanSwapChain(VulkanSwapChain &&) noexcept;

        inline std::any getSwapChain() override
        {
            return _swapchain;
        };

        virtual ~VulkanSwapChain();

    private:
        VkSwapchainKHR _swapchain{VK_NULL_HANDLE};
        // for destroy swapchain and copy ctor
        std::shared_ptr<ILogicalDevice> _device;
        std::shared_ptr<IRenderingSurface> _surface;
        VkPresentModeKHR _presentMode;
        uint32_t _imageCount;
        std::set<VkImageUsageFlagBits> _imageUsageBits;
        VkSurfaceTransformFlagBitsKHR _transform;
        VkSurfaceFormatKHR _surfaceFormat;
        uint32_t _imageLayers;
    };
}