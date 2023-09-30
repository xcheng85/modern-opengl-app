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
}