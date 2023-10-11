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
    // deps
    // array of attachments
    // array of subpasses
    // deps between subpasses


    class ILogicalDevice;
    class IRenderPass : public VulkanObject<VkRenderPass, VK_OBJECT_TYPE_RENDER_PASS>
    {
    public:
        virtual ~IRenderPass() noexcept = default;
    };

    class VulkanRenderPass: public IRenderPass
    {
    public:
        VulkanRenderPass(std::shared_ptr<ILogicalDevice>);
        virtual ~VulkanRenderPass();

    private:
        std::shared_ptr<ILogicalDevice> _device;
    };
}