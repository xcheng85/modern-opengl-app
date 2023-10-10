#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <volk.h>
#include "di.hpp"
#include "utils.h"
#include "vulkan.h"

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
    class ISwapChain : public VulkanObject<VkSwapchainKHR, VK_OBJECT_TYPE_SWAPCHAIN_KHR>
    {
    public:
        virtual ~ISwapChain() noexcept = default;
        // virtual std::any getSwapChain() = 0;
        // // explicit ISwapChain(ISwapChain &&) = delete;
    };

    class ILogicalDevice;
    class IRenderingSurface;

    class VulkanSwapChain : public ISwapChain
    {
    public:
        explicit VulkanSwapChain() = delete;
        explicit VulkanSwapChain(
            std::shared_ptr<ILogicalDevice>,
            std::shared_ptr<IRenderingSurface>,
            VkPresentModeKHR const &,
            const uint32_t,
            std::set<VkImageUsageFlagBits> const &,
            VkSurfaceTransformFlagBitsKHR const &,
            VkSurfaceFormatKHR const &,
            const uint32_t,
            VulkanSwapChain* old = nullptr);
        // resize
        // ctor overload
        explicit VulkanSwapChain(ISwapChain &);
        explicit VulkanSwapChain(VulkanSwapChain &);
        explicit VulkanSwapChain(VulkanSwapChain &&) noexcept;

        // inline std::any getSwapChain() override
        // {
        //     return _swapchain;
        // };

        virtual ~VulkanSwapChain();

        // image acquisition
        // return the image index in the swap chain that you can write into
        // semaphore: sync device queue
        // fence: sync application (cpu/host)
        VkResult acquireNextImage(uint32_t &image_index, VkSemaphore image_acquired_semaphore, VkFence fence = VK_NULL_HANDLE) const;
        // get the images in the swapchain
        inline const std::vector<VkImage> &getImages() const
        {
            return _images;
        };
        inline auto const &getSurfaceFormat() const
        {
            return _surfaceFormat;
        };

        inline auto const &getImageViews() const
        {
            return _image_views;
        };

        inline auto const &getImageExtents() const
        {
            return _imageExtent;
        };

    private:
        // VkSwapchainKHR _swapchain{VK_NULL_HANDLE};
        // for destroy swapchain and copy ctor
        std::shared_ptr<ILogicalDevice> _device;
        std::shared_ptr<IRenderingSurface> _surface;
        VkPresentModeKHR _presentMode;
        uint32_t _imageCount;
        std::set<VkImageUsageFlagBits> _imageUsageBits;
        VkSurfaceTransformFlagBitsKHR _transform;
        VkSurfaceFormatKHR _surfaceFormat;
        uint32_t _imageLayers;
        VkExtent2D _imageExtent;

        std::vector<VkImage> _images;
        // image is only accessible through imageView, last step
        std::vector<VkImageView> _image_views;
    };
}