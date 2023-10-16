#include <format>
#include <iostream>
#include <algorithm>
#include "commandpool.h"
#include "device.h"
#include "queue.h"

using namespace std;
namespace SharedUtils
{
    VulkanCommandPool::VulkanCommandPool(
        std::shared_ptr<ILogicalDevice> device,
        uint32_t queueFamilyIndex,
        size_t threadId)
        : _device(device),
          _threadId(threadId)
    {
        cout << format("--> VulkanCommandPool::VulkanCommandPool") << std::endl;
        // three pool and three command buffers
        VkCommandPoolCreateInfo command_pool_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, /* Allow command from this pool buffers to be reset*/
            .queueFamilyIndex = queueFamilyIndex,
        };

        VK_CHECK(vkCreateCommandPool(_device->getVkHandle(), &command_pool_info, nullptr, &_handle));
        cout << format("<-- VulkanCommandPool::VulkanCommandPool") << std::endl;
    }

    VulkanCommandPool::~VulkanCommandPool()
    { 
        if (_handle != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(_device->getVkHandle(), _handle, nullptr);
        }
    }
}
