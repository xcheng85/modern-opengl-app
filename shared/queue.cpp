#include <format>
#include <iostream>
#include <algorithm>
#include <string.h> // strcmp
#include <ranges>
#include "queue.h"
#include "device.h"
#include "utils.h"

using namespace std;

namespace SharedUtils
{
    VulkanLogicDeviceQueue::VulkanLogicDeviceQueue(const VkDevice &vkLogicDevice, uint32_t familyIndex, uint32_t queueIndex)
    {
        // to do queueIndex fix bug
        cout << format("--> VulkanLogicDeviceQueue::VulkanLogicDeviceQueue") << std::endl;
        // auto vkLogicDevice = any_cast<VkDevice>(logicDevice->getDevice());
        vkGetDeviceQueue(vkLogicDevice, familyIndex, queueIndex, &_queue);
        cout << format("<-- VulkanLogicDeviceQueue::VulkanLogicDeviceQueue") << std::endl;
    }

    VulkanLogicDeviceQueue::~VulkanLogicDeviceQueue()
    {
    }

    VulkanLogicDeviceQueueList::VulkanLogicDeviceQueueList(std::shared_ptr<ILogicalDevice> logicDevice)
    {
        cout << format("--> VulkanLogicDeviceQueueList::VulkanLogicDeviceQueueList") << std::endl;
        auto vkLogicDevice = any_cast<VkDevice>(logicDevice->getDevice());
        auto queueInfo = any_cast<vector<QueueInfo>>(logicDevice->getQueueInfo());

        for (auto const &f : queueInfo)
        {
            //for (auto const &q : f.queueIndexes)
            for (uint32_t queueIndex = 0U; queueIndex < f.queueIndexes.size(); ++queueIndex)
            {
                _queues.push_back(make_shared<VulkanLogicDeviceQueue>(vkLogicDevice, f.familyIndex, queueIndex));
            }
        }

        cout << format("<-- VulkanLogicDeviceQueueList::VulkanLogicDeviceQueueList") << std::endl;
    }

    VulkanLogicDeviceQueueList::~VulkanLogicDeviceQueueList()
    {
    }
}
