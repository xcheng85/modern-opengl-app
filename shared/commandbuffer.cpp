#include <format>
#include <iostream>
#include <algorithm>
#include "commandbuffer.h"
#include "commandpool.h"
#include "device.h"

using namespace std;
namespace SharedUtils
{
    VulkanCommandBuffer::VulkanCommandBuffer(
        std::shared_ptr<ILogicalDevice> device,
        std::shared_ptr<ICommandPool> commandPool) : _device(device),
                                                     _commandPool(commandPool)
    {
        cout << format("--> VulkanCommandBuffer::VulkanCommandBuffer") << std::endl;
        // 1 command buffer for each swapchain image
        const VkCommandBufferAllocateInfo command_buffer_ai = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool->getVkHandle(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        VK_CHECK(vkAllocateCommandBuffers(device->getVkHandle(), &command_buffer_ai, &_handle));
        cout << format("<-- VulkanCommandBuffer::VulkanCommandBuffer") << std::endl;
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        vkFreeCommandBuffers(_device->getVkHandle(), _commandPool->getVkHandle(), 1, &_handle);
    }

    void VulkanCommandBuffer::beginRenderPass()
    {
        VkRenderPassBeginInfo rp_begin{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = this->render_pass,
            .framebuffer = framebuffer,
            .renderArea{
                .extent{
                    .width = extents.width,
                    .height = extents.height,
                }},
            .clearValueCount = 1,
            .pClearValues = &clear_value,

        };
        vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    }
}
