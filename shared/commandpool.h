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
    // design 1: all the swapchain images shared the same command pool, no multiple threading usage

    // design 2: each swapchain image use separate command pool, multiple-threading possible

    // each thread must have separate command pool
    // all deps:
    // threadId
    // logic device
    // queue family index

    // own vector of commandbuffers

    // command pool is owned by logic device

    class ILogicalDevice;
    class ICommandBuffer;
    class ICommandPool : public VulkanObject<VkCommandPool, VK_OBJECT_TYPE_COMMAND_POOL>
    {
    public:
        virtual ~ICommandPool() noexcept = default;
    };

    class VulkanCommandPool : public ICommandPool
    {
    public:
        VulkanCommandPool(std::shared_ptr<ILogicalDevice>, uint32_t queueFamilyIndex, size_t threadId = 0);
        virtual ~VulkanCommandPool();

    private:
        std::shared_ptr<ILogicalDevice> _device;

        size_t _threadId{0};

        uint32_t _queueFamilyIndex;

     //   std::vector<std::unique_ptr<ICommandBuffer>> _commandBuffers;
    };
}