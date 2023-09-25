#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <volk.h>

#include "glslang_c_interface.h"
#include "resource_limits_c.h"

#include "di.hpp"
#include "utils.h"

namespace SharedUtils
{
    // ioc
    class IRenderingContext
    {
    public:
        virtual ~IRenderingContext() noexcept = default;
        virtual const VkInstance getInstance() const = 0;
    };
    class IRenderingHostAppSettings
    {
    public:
        virtual ~IRenderingHostAppSettings() noexcept = default;
        virtual std::string name() = 0;
        virtual std::string version() = 0;
    };
    class IRenderingDebugger
    {
    public:
        virtual ~IRenderingDebugger() noexcept = default;
    };

    // vulkan imp
    static auto VALIDATION_LAYERS = [] {};
    static auto INSTANCE_EXTENSIONS = [] {};
    class VulkanRenderingContext : public IRenderingContext
    {
    public:
        BOOST_DI_INJECT(VulkanRenderingContext,
                        (named = VALIDATION_LAYERS) std::vector<std::string> const &,
                        (named = INSTANCE_EXTENSIONS) std::vector<std::string> const &,
                        IRenderingHostAppSettings &);
        // VulkanRenderingContext(IRenderingContextValidationLayer &, IRenderingContextExtensions &, IRenderingHostAppSettings &);
        inline const VkInstance getInstance() const override
        {
            return this->_instance;
        }

    private:
        // dispatchable handle: pointer
        VkInstance _instance;
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
    class VulkanRenderingDebugger : public IRenderingDebugger
    {
    public:
        VulkanRenderingDebugger(IRenderingContext const &ctx);

    private:
        VkDebugUtilsMessengerEXT _messenger;
        VkDebugReportCallbackEXT _reportCallback;
    };

    struct ShaderModule final
    {
        // unsigned int is required by glslang_program_SPIRV_get
        std::vector<unsigned int> SPIRV;
        VkShaderModule shaderModule = nullptr;
    };

    // compile and then create
    VkResult createShaderModuleFromSource(VkDevice device, ShaderModule *shader, const char *fileName);
    // from pre-compiled
    VkResult createShaderModuleFromSpv(VkDevice device, ShaderModule *shader, const char *spvfileName);

    size_t compileShaderFile(const char *file, ShaderModule &shaderModule);

    VkShaderStageFlagBits glslangShaderStageToVulkan(glslang_stage_t sh);

    VkResult createGraphicsPipeline(VkDevice device);
}