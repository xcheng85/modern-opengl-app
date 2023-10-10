#include <malloc.h>
#include <string.h>
#include <string>
#include <cassert>
#include <numeric>
#include <vector>
#include <iostream>
#include <format>

#include "utils.h"
#include "instance.h"

using namespace std;

namespace SharedUtils
{

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
        VkDebugUtilsMessageTypeFlagsEXT Type,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *UserData)
    {
        std::cerr << format("validation layer: {}", pCallbackData->pMessage) << std::endl;
        return VK_FALSE;
    }
    // on vulkan object
    // objectType
    // object handle
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objectType,
        uint64_t object,
        size_t location,
        int32_t messageCode,
        const char *pLayerPrefix,
        const char *pMessage,
        void *UserData)
    {
        if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
            return VK_FALSE;
        std::cerr << format("Debug callback: {}, {}", pLayerPrefix, pMessage) << std::endl;
        return VK_FALSE;
    }

    VulkanRenderingHostAppSettings::VulkanRenderingHostAppSettings(const std::string name, const std::string version)
        : _name(name), _version(version)
    {
    }

    VulkanInstance::VulkanInstance(
        std::vector<std::string> const &validationLayers,
        std::vector<std::string> const &instanceExtensions,
        IRenderingHostAppSettings &hostAppSettings)
    {
        cout << format("--> VulkanInstance::VulkanInstance") << std::endl;

        const VkApplicationInfo appinfo =
            {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = hostAppSettings.name().c_str(),
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "No Engine",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_3};

        // auto allLayers = std::accumulate(
        // 					 std::next(validationLayers.getValidationLayers().begin()), validationLayers.getValidationLayers().end(),
        // 					 validationLayers.getValidationLayers()[0],
        // 					 [](std::string a, std::string b)
        // 					 {
        // 						 return a + b;
        // 					 })
        // 					 .c_str();
        // auto allExts = std::accumulate(
        // 				   std::next(extensions.getExtensions().begin()), extensions.getExtensions().end(),
        // 				   extensions.getExtensions()[0],
        // 				   [](std::string a, std::string b)
        // 				   {
        // 					   return a + b;
        // 				   })
        // 				   .c_str();
        std::vector<const char *> layer_names(validationLayers.size());
        // std::transform(validationLayers.getValidationLayers().begin(), validationLayers.getValidationLayers().end(), layer_names.begin(), [](const std::string &s)
        // 			   { return s.c_str(); });
        std::vector<const char *> extensitions_names(instanceExtensions.size());
        // std::transform(extensions.getExtensions().begin(), extensions.getExtensions().end(), extensitions_names.begin(), [](const std::string &s)
        // 			   { return s.c_str(); });
        convert(validationLayers, layer_names);
        convert(instanceExtensions, extensitions_names);

        const VkInstanceCreateInfo createInfo =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .pApplicationInfo = &appinfo,
                .enabledLayerCount = static_cast<uint32_t>(layer_names.size()),
                .ppEnabledLayerNames = layer_names.data(),
                .enabledExtensionCount = static_cast<uint32_t>(extensitions_names.size()),
                .ppEnabledExtensionNames = extensitions_names.data()};

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &_handle));
        // This function will load all required Vulkan entrypoints, including all extensions; you can use Vulkan from here on as usual.
        volkLoadInstance(_handle);
        // add vulkan debugger layer
        {
            const VkDebugUtilsMessengerCreateInfoEXT ci = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = &VulkanDebugCallback,
                .pUserData = nullptr};
            VK_CHECK(vkCreateDebugUtilsMessengerEXT(_handle, &ci, nullptr, &_messenger));
        }
        // enable the following will crash
        {

            const VkDebugReportCallbackCreateInfoEXT ci = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags =
                    VK_DEBUG_REPORT_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_ERROR_BIT_EXT |
                    VK_DEBUG_REPORT_DEBUG_BIT_EXT,
                .pfnCallback = &VulkanDebugReportCallback,
                .pUserData = nullptr};
            VK_CHECK(vkCreateDebugReportCallbackEXT(_handle, &ci, nullptr, &_reportCallback));
        }

        cout << format("<-- VulkanInstance::VulkanInstance") << std::endl;
    }

    VulkanInstance::~VulkanInstance()
    {
        vkDestroyDebugReportCallbackEXT(_handle, _reportCallback, nullptr);
    }
}