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
    class IInstance: public VulkanObject<VkInstance, VK_OBJECT_TYPE_INSTANCE>
    {
    public:
        virtual ~IInstance() noexcept = default;
    };

    class IRenderingHostAppSettings
    {
    public:
        virtual ~IRenderingHostAppSettings() noexcept = default;
        virtual std::string name() = 0;
        virtual std::string version() = 0;
    };

    // vulkan imp
    static auto VALIDATION_LAYERS = [] {};
    static auto INSTANCE_EXTENSIONS = [] {};
    class VulkanInstance : public IInstance
    {
    public:
        BOOST_DI_INJECT(VulkanInstance,
                        (named = VALIDATION_LAYERS) std::vector<std::string> const &,
                        (named = INSTANCE_EXTENSIONS) std::vector<std::string> const &,
                        IRenderingHostAppSettings &);
        virtual ~VulkanInstance();

    private:
        // debugger
        VkDebugUtilsMessengerEXT _messenger;
        VkDebugReportCallbackEXT _reportCallback;
    };

    static auto APP_NAME = [] {};
    static auto APP_VERSION = [] {};

    class VulkanRenderingHostAppSettings : public IRenderingHostAppSettings
    {
    public:
        BOOST_DI_INJECT(VulkanRenderingHostAppSettings, (named = APP_NAME) const std::string name, (named = APP_VERSION) const std::string version);
        inline std::string name() override
        {
            return _name;
        };
        inline std::string version() override
        {
            return _version;
        };

    private:
        std::string _name;
        std::string _version;
    };
}