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
    // deps:
    // 1. commandPool
    class ICommandPool;
    class ILogicalDevice;
    class IRenderPass;
    class IFrameBuffer;
    class ICommandBuffer : public VulkanObject<VkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER>
    {
    public:
        virtual ~ICommandBuffer() noexcept = default;
    };

    // vkCmd used:
    // vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    // vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);
    //  vkCmdSetViewport(cmd, 0, 1, &vp);
    //           vkCmdSetScissor(cmd, 0, 1, &scissor);
    //             vkCmdDraw(cmd, 3, 1, 0, 0);
    //             vkCmdEndRenderPass(cmd);

    class VulkanCommandBuffer : public ICommandBuffer
    {
    public:
        VulkanCommandBuffer(std::shared_ptr<ILogicalDevice>, std::shared_ptr<ICommandPool>);
        virtual ~VulkanCommandBuffer();

        void beginRenderPass();
        // void bindPipeline(VkPipelineBindPoint pipeline_bind_point);
        // void setViewPort(uint32_t first_viewport, const std::vector<VkViewport> &viewports);
        // void setScissor(uint32_t first_scissor, const std::vector<VkRect2D> &scissors);
        // void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
        // void endRenderPass();

        struct RenderPassBinding
        {
            const IRenderPass *renderPass; // layout in and out, attachment, etc.
            const IFrameBuffer *frameBuffer; // image view and attachement
        };

    private:
        std::shared_ptr<ILogicalDevice> _device;
        std::shared_ptr<ICommandPool> _commandPool;
    };
}