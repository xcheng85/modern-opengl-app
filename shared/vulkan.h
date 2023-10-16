#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <volk.h>

#include "glslang_c_interface.h"
#include "resource_limits_c.h"

#include "di.hpp"
#include "utils.h"

namespace SharedUtils
{
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

    template <typename VkHandle, VkObjectType OBJECT_TYPE>
    class VulkanObject
    {
    public:
        VulkanObject(VkHandle handle = VK_NULL_HANDLE) : _handle{handle}
        {
        }

        VulkanObject(const VulkanObject &) = delete;
        VulkanObject &operator=(const VulkanObject &) = delete;

        VulkanObject(VulkanObject &&other) : _handle{other._handle}
        {
            other._handle = VK_NULL_HANDLE;
        }

        VulkanObject &operator=(VulkanObject &&other)
        {
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
            return *this;
        }

        virtual ~VulkanObject() = default;

        inline VkObjectType getObjectType() const
        {
            return OBJECT_TYPE;
        }

        inline const VkHandle &getVkHandle() const
        {
            return _handle;
        }

    protected:
        VkHandle _handle;
    };
    
    inline bool isDepthFormat(VkFormat format)
    {
        return format == VK_FORMAT_D16_UNORM ||
               format == VK_FORMAT_D32_SFLOAT;
    }

    inline bool isDepthStencilFormat(VkFormat format)
    {
        return format == VK_FORMAT_D16_UNORM_S8_UINT ||
               format == VK_FORMAT_D24_UNORM_S8_UINT ||
               format == VK_FORMAT_D32_SFLOAT_S8_UINT;
    }
}
