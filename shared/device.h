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
    class IRenderingSurface;
    class IPhysicalDevice
    {
    public:
        virtual ~IPhysicalDevice() noexcept = default;
        virtual std::any getDevice() = 0;
        virtual std::any getProperties() = 0;
        virtual bool isExtensionSupported(std::string const &requested_extension) const = 0;
    };

    class IPhysicalDeviceList
    {
    public:
        virtual ~IPhysicalDeviceList() noexcept = default;
        inline auto getDevices() const
        {
            return _devices;
        };

    protected:
        std::vector<std::shared_ptr<IPhysicalDevice>> _devices;
    };

    class VulkanPhysicalDevice : public IPhysicalDevice
    {
    public:
        explicit VulkanPhysicalDevice() = delete;
        explicit VulkanPhysicalDevice(VkPhysicalDevice const &);
        virtual ~VulkanPhysicalDevice();
        std::any getDevice() override;
        std::any getProperties() override;
        bool isExtensionSupported(std::string const &requested_extension) const override;
        VkBool32 isPresentSupported(VkSurfaceKHR surface, uint32_t queue_family_index) const;
        inline auto const &getQueueFamilyProperties() const
        {
            return _queue_family_properties;
        };

    private:
        VkPhysicalDevice _device;
        // uniform init
        VkPhysicalDeviceFeatures _features{};
        VkPhysicalDeviceProperties _properties;
        VkPhysicalDeviceMemoryProperties _memory_properties;
        std::vector<VkQueueFamilyProperties> _queue_family_properties;
        // device extensions
        std::vector<VkExtensionProperties> _extensions{};
    };

    class IRenderingContext;
    class VulkanPhysicalDeviceList : public IPhysicalDeviceList
    {
    public:
        explicit VulkanPhysicalDeviceList(std::shared_ptr<IRenderingContext>);
        virtual ~VulkanPhysicalDeviceList();
    };

    class ILogicalDevice
    {
    public:
        virtual ~ILogicalDevice() noexcept = default;
        virtual std::any getDevice() = 0;
        virtual std::any getQueueInfo() = 0;
    };

    struct QueueInfo;
    static auto DESIRE_PHYSICAL_DEVICE_EXTS = [] {};
    static auto DESIRE_PHYSICAL_DEVICE_FEATURES = [] {};
    static auto DESIRE_QUEUE_FAMILY_CAPABILITY = [] {};
    class VulkanLogicalDevice : public ILogicalDevice
    {
    public:
        // requested device extensions, and feature
        // device ext: std::vector
        // features: VkPhysicalDeviceFeatures
        // IRenderingSurface: vulkanSurface to differentiate cpu and gpu cap
        BOOST_DI_INJECT(VulkanLogicalDevice, std::shared_ptr<IPhysicalDeviceList>,
                        (named = DESIRE_PHYSICAL_DEVICE_EXTS) std::vector<std::string> const &,
                        (named = DESIRE_PHYSICAL_DEVICE_FEATURES) VkPhysicalDeviceFeatures const &,
                        (named = DESIRE_QUEUE_FAMILY_CAPABILITY) VkQueueFlagBits,
                        std::shared_ptr<IRenderingSurface>);

        // explicit VulkanLogicalDevice(std::shared_ptr<IPhysicalDevice>, std::shared_ptr<IRenderingSurface>);
        virtual ~VulkanLogicalDevice();

        inline std::any getDevice() override
        {
            return _device;
        }

        inline any getQueueInfo() override {
            return _queueInfo;
        }

    private:
        VkDevice _device;
        std::vector<QueueInfo> _queueInfo;
    };
}