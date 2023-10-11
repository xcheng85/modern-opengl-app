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

    class VulkanFrameBuffer : public IFrameBuffer
    {
    public:
        VulkanFrameBuffer(std::shared_ptr<ILogicalDevice>);
        virtual ~VulkanFrameBuffer();

    private:
        std::shared_ptr<ILogicalDevice> _device;
    };
}