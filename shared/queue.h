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
    // queue --> family --> logical device
    class IDeviceQueue
    {
    public:
        virtual ~IDeviceQueue() noexcept = default;
    };

    class IDeviceQueueList
    {
    public:
        virtual ~IDeviceQueueList() noexcept = default;

    protected:
        std::vector<std::shared_ptr<IDeviceQueue>> _queues;
    };

    struct QueueInfo
    {
        uint32_t familyIndex;
        std::vector<uint32_t> queueIndexes;
        std::vector<float> priorities;
    };

    class VulkanLogicDeviceQueue : public IDeviceQueue
    {
    public:
        explicit VulkanLogicDeviceQueue() = delete;
        explicit VulkanLogicDeviceQueue(const VkDevice &, uint32_t familyIndex, uint32_t queueIndex);
        virtual ~VulkanLogicDeviceQueue();
        inline auto &getVKQueue() noexcept
        {
            return _queue;
        };
        inline auto &getFamilyIndex() noexcept
        {
            return _familyIndex;
        };

    private:
        VkQueue _queue;
        uint32_t _familyIndex; // needed for command pool creation
    };

    class ILogicalDevice;
    class VulkanLogicDeviceQueueList : public IDeviceQueueList
    {
    public:
        explicit VulkanLogicDeviceQueueList() = delete;
        explicit VulkanLogicDeviceQueueList(std::shared_ptr<ILogicalDevice>);
        virtual ~VulkanLogicDeviceQueueList();
    };
}