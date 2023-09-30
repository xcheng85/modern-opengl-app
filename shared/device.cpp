#include <format>
#include <iostream>
#include <algorithm>
#include <string.h> // strcmp
#include <ranges>
#include "device.h"
#include "surface.h"
#include "instance.h"
#include "queue.h"
#include "utils.h"

using namespace std;

namespace SharedUtils
{
    VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice const &device) : _device(device)
    {
        uint32_t device_extension_count;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, nullptr));

        _extensions.resize(device_extension_count);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, _extensions.data()));

        vkGetPhysicalDeviceFeatures(device, &_features);
        vkGetPhysicalDeviceProperties(device, &_properties);
        vkGetPhysicalDeviceMemoryProperties(device, &_memory_properties);
        uint32_t queue_family_properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_properties_count, nullptr);
        _queue_family_properties.resize(queue_family_properties_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_properties_count, _queue_family_properties.data());

        cout << format("Found Device: {}", _properties.deviceName) << endl;
        // Display supported extensions
        if (_extensions.size() > 0)
        {
            cout << format("Device: {} supports the following extensions:", _properties.deviceName) << endl;
            for (auto &extension : _extensions)
            {
                cout << format("{}", extension.extensionName) << endl;
            }
        }
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
    }

    std::any VulkanPhysicalDevice::getDevice()
    {
        return this->_device;
    }

    std::any VulkanPhysicalDevice::getProperties()
    {
        return this->_properties;
    }

    bool VulkanPhysicalDevice::isExtensionSupported(std::string const &extension) const
    {
        // lamda, capture by value: extension
        return std::find_if(_extensions.begin(), _extensions.end(),
                            [extension](auto &ext)
                            {
                                return strcmp(ext.extensionName, extension.c_str()) == 0;
                            }) != _extensions.end();
    }

    VkBool32 VulkanPhysicalDevice::isPresentSupported(VkSurfaceKHR surface, uint32_t queue_family_index) const
    {
        VkBool32 present_supported;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(_device, queue_family_index, surface, &present_supported));
        return present_supported;
    }

    VulkanPhysicalDeviceList::VulkanPhysicalDeviceList(std::shared_ptr<IInstance> instance)
    {
        cout << format("--> VulkanPhysicalDeviceList::VulkanPhysicalDeviceList") << std::endl;

        auto const &vkInstance = any_cast<VkInstance>(instance->getInstance());
        uint32_t physical_device_count{0};
        VK_CHECK(vkEnumeratePhysicalDevices(vkInstance, &physical_device_count, nullptr));

        if (physical_device_count < 1)
        {
            throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
        }

        std::vector<VkPhysicalDevice> physical_devices;
        physical_devices.resize(physical_device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(vkInstance, &physical_device_count, physical_devices.data()));

        // Create gpus wrapper objects from the VkPhysicalDevice's
        for (auto &physical_device : physical_devices)
        {
            _devices.push_back(make_shared<VulkanPhysicalDevice>(physical_device));
        }

        cout << format("<-- VulkanPhysicalDeviceList::VulkanPhysicalDeviceList") << std::endl;
    }

    VulkanPhysicalDeviceList::~VulkanPhysicalDeviceList()
    {
    }

    VulkanLogicalDevice::VulkanLogicalDevice(std::shared_ptr<IPhysicalDeviceList> pdList, vector<string> const &desiredExts,
                                             VkPhysicalDeviceFeatures const &features, VkQueueFlagBits cap,
                                             std::shared_ptr<IRenderingSurface> rendSurface)
    {
        cout << format("--> VulkanLogicalDevice::VulkanLogicalDevice") << std::endl;
        auto devices = pdList->getDevices();
        for (auto &pDevice : devices)
        {
            auto deviceProperties = any_cast<VkPhysicalDeviceProperties>(pDevice->getProperties());
            cout << format("deviceName: {}", deviceProperties.deviceName) << endl;
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                continue;
            }

            vector<const char *> supported{};
            vector<const char *> unsupported{};
            for (auto &extension : desiredExts)
            {
                if (pDevice->isExtensionSupported(extension))
                {
                    supported.emplace_back(extension.c_str());
                }
                else
                {
                    unsupported.emplace_back(extension.c_str());
                }
            }

            if (unsupported.size() > 0)
            {
                for (auto &ext : unsupported)
                {
                    cout << format("{} is not supported by physical device", ext) << endl;
                }
                continue;
            }

            // check cap for presenting wsi surface

            // check device features, how to compare two struct instance easily ?

            // choice of queue family
            // advanced algorithm needs multiple queues.
            // ignore the queue family with 0 queue size

            // selected the device successfully
            auto const &vkPDevices = std::static_pointer_cast<VulkanPhysicalDevice>(pDevice);
            auto const &vkSurface = std::any_cast<VkSurfaceKHR>(rendSurface->getSurface());
            auto const &queueFamilies = vkPDevices->getQueueFamilyProperties();

            // ask for create multiple queues for one family within a device
            // queue in family will be created when we create the logical device
            // std::vector<QueueInfo> queueInfoFiltered;
            uint32_t familyIndex{0};
            for (auto const &qf : queueFamilies)
            {
                QueueInfo tmp;
                tmp.familyIndex = familyIndex++;

                if (qf.queueCount > 0 && qf.queueFlags & cap)
                {
                    for (uint32_t queue_idx = 0; queue_idx < qf.queueCount; queue_idx++)
                    {
                        // ADM RADV RENOIR can support this surface as well
                        if (vkPDevices->isPresentSupported(vkSurface, queue_idx))
                        {
                            tmp.priorities.push_back(0.5f);
                            tmp.queueIndexes.push_back(queue_idx);
                        }
                    }
                    _queueInfo.push_back(tmp);
                }
            }
            // uniform init
            auto queueInfoSize = _queueInfo.size();
            if (0 == queueInfoSize)
            {
                continue;
            }
            vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueInfoSize, {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO});
            uint32_t currIdx{0};
            for (auto const &qf : _queueInfo)
            {
                auto &queue_create_info = queueCreateInfos[currIdx++];
                queue_create_info.queueFamilyIndex = qf.familyIndex;
                queue_create_info.queueCount = qf.priorities.size();
                queue_create_info.pQueuePriorities = qf.priorities.data();
            }

            VkDeviceCreateInfo logicalDeviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
            logicalDeviceCreateInfo.pNext = nullptr;
            logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            logicalDeviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
            logicalDeviceCreateInfo.enabledExtensionCount = supported.size();
            logicalDeviceCreateInfo.ppEnabledExtensionNames = supported.data();
            logicalDeviceCreateInfo.pEnabledFeatures = &features;

            this->_pDevice = std::any_cast<VkPhysicalDevice>(pDevice->getDevice());
            VK_CHECK(vkCreateDevice(_pDevice, &logicalDeviceCreateInfo, nullptr, &_device));
            break;
        }
        cout << format("<-- VulkanLogicalDevice::VulkanLogicalDevice") << endl;
    }

    VulkanLogicalDevice::~VulkanLogicalDevice()
    {
        // destroy logical device
        vkDestroyDevice(this->_device, nullptr);
        _device = VK_NULL_HANDLE;
    }
}
