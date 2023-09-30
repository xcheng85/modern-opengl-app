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
    };

    class ILogicalDevice;
    class IRenderingSurface;

    static auto DESIRE_SWAPCHAIN_PRESENT_MODE = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_COUNT = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_EXTENT = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_USAGE = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_TRANSFORMATION = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_FORMAT = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_LAYERS = [] {};

    class VulkanSwapChain : public ISwapChain
    {
    public:
        explicit VulkanSwapChain() = delete;
        BOOST_DI_INJECT(VulkanSwapChain,
                        std::shared_ptr<ILogicalDevice>,
                        std::shared_ptr<IRenderingSurface>,
                        (named = DESIRE_SWAPCHAIN_PRESENT_MODE) VkPresentModeKHR const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_COUNT) const uint32_t,
                        (named = DESIRE_SWAPCHAIN_IMAGES_USAGE) std::set<VkImageUsageFlagBits> const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_TRANSFORMATION) VkSurfaceTransformFlagBitsKHR const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_FORMAT) VkSurfaceFormatKHR const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_LAYERS) const uint32_t);
        // resize
        // ctor overload
        explicit VulkanSwapChain(ISwapChain &);
        explicit VulkanSwapChain(VulkanSwapChain &);

        virtual ~VulkanSwapChain();

    private:
        VkSwapchainKHR _swapchain;
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