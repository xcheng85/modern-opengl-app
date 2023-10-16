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

namespace SharedUtils
{
    class ILogicalDevice;
    class IFrameBuffer : public VulkanObject<VkFramebuffer, VK_OBJECT_TYPE_FRAMEBUFFER>
    {
    public:
        virtual ~IFrameBuffer() noexcept = default;
    };

    // Two use cases of framebuffer
    // 1. for the swapchain images: created not using resource cache pool and managed in the application class

    // 2. for any other intermediate: managed by resource cache pool

    // framebuffer are used together with render pass
    // framebuffer specify image resource to be used as the attachments in the renderpass
    // size of render area

    // framebuffer deps:
    // 1. imageview (to the attachment of renderpass)
    // 2. renderpass

    class VulkanFrameBuffer : public IFrameBuffer
    {
    public:
        VulkanFrameBuffer(std::shared_ptr<ILogicalDevice>);
        virtual ~VulkanFrameBuffer();

    private:
        std::shared_ptr<ILogicalDevice> _device;
    };
}