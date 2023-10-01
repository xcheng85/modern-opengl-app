#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <volk.h>
#include "di.hpp"
#include "utils.h"

namespace SharedUtils
{
    class ILogicalDevice;
    class ISwapChain;
    class IRenderingSurface;
    class IContext
    {
    public:
        virtual ~IContext() noexcept = default;
        virtual void resize(const uint32_t width, const uint32_t height) = 0;
    };

    // life cycle of swapchain could be unique_ptr managed here
    // vulkan context is injected into Application.
    // vulkan context should depends on logic device and wsi surface
    // bunch of
    static auto DESIRE_SWAPCHAIN_PRESENT_MODE = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_COUNT = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_EXTENT = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_USAGE = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_TRANSFORMATION = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_FORMAT = [] {};
    static auto DESIRE_SWAPCHAIN_IMAGES_LAYERS = [] {};
    static auto OLD_SWAPCHAIN = [] {};
    class VulkanContext : public IContext
    {
    public:
        BOOST_DI_INJECT(VulkanContext,
                        std::shared_ptr<ILogicalDevice>,
                        std::shared_ptr<IRenderingSurface>,
                        (named = DESIRE_SWAPCHAIN_PRESENT_MODE) VkPresentModeKHR const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_COUNT) const uint32_t,
                        (named = DESIRE_SWAPCHAIN_IMAGES_USAGE) std::set<VkImageUsageFlagBits> const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_TRANSFORMATION) VkSurfaceTransformFlagBitsKHR const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_FORMAT) VkSurfaceFormatKHR const &,
                        (named = DESIRE_SWAPCHAIN_IMAGES_LAYERS) const uint32_t);
        virtual ~VulkanContext();
        void resize(const uint32_t width, const uint32_t height) override;

    private:
        std::shared_ptr<ILogicalDevice> _device;
        std::shared_ptr<IRenderingSurface> _surface;
        std::unique_ptr<ISwapChain> _swapchain;
    };

}