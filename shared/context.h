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
    class VulkanContext : public IContext
    {
    public:
        BOOST_DI_INJECT(VulkanContext,
                        std::shared_ptr<ILogicalDevice>,
                        std::shared_ptr<IRenderingSurface>,
                        std::unique_ptr<ISwapChain>);
        virtual ~VulkanContext();
        void resize(const uint32_t width, const uint32_t height) override;

    private:
        std::shared_ptr<ILogicalDevice> _device;
        std::shared_ptr<IRenderingSurface> _surface;
        std::unique_ptr<ISwapChain> _swapchain;
    };

}