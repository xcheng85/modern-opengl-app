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
    // // ioc
    // class IPhysicalDevice
    // {
    // public:
    //     virtual ~IPhysicalDevice() noexcept = default;
    // };

    class IPhysicalDevices
    {
    public:
        virtual ~IPhysicalDevices() noexcept = default;
    };

    // class VulkanPhysicalDevice
    // {
    // public:
    //     explicit VulkanPhysicalDevice(VkPhysicalDevice const&);
    //     virtual ~VulkanPhysicalDevice();
    // private:
    //     VkPhysicalDevice _device;
    // };
    class IRenderingContext;
    class VulkanPhysicalDevices : public IPhysicalDevices
    {
    public:
        explicit VulkanPhysicalDevices(std::shared_ptr<IRenderingContext>);
        virtual ~VulkanPhysicalDevices();

    private:
        //std::vector<std::unique_ptr<IPhysicalDevice>> _devices;
    };

}