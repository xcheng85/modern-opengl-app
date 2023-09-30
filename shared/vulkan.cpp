#include <malloc.h>
#include <string.h>
#include <string>
#include <cassert>
#include <numeric>
#include <vector>
#include <iostream>
#include <format>

#include "utils.h"
#include "vulkan.h"

using namespace std;

namespace SharedUtils
{
	glslang_stage_t glslangShaderStageFromFileName(const char *fileName)
	{
		if (endsWith(fileName, ".vert"))
			return GLSLANG_STAGE_VERTEX;

		if (endsWith(fileName, ".frag"))
			return GLSLANG_STAGE_FRAGMENT;

		if (endsWith(fileName, ".geom"))
			return GLSLANG_STAGE_GEOMETRY;

		if (endsWith(fileName, ".comp"))
			return GLSLANG_STAGE_COMPUTE;

		if (endsWith(fileName, ".tesc"))
			return GLSLANG_STAGE_TESSCONTROL;

		if (endsWith(fileName, ".tese"))
			return GLSLANG_STAGE_TESSEVALUATION;

		return GLSLANG_STAGE_VERTEX;
	}

	static size_t compileShader(glslang_stage_t stage, const char *shaderSource, ShaderModule &shaderModule)
	{
		const glslang_input_t input =
			{
				.language = GLSLANG_SOURCE_GLSL, // GLSL, HSLS
				.stage = stage,
				.client = GLSLANG_CLIENT_VULKAN,
				.client_version = GLSLANG_TARGET_VULKAN_1_1,
				.target_language = GLSLANG_TARGET_SPV,
				.target_language_version = GLSLANG_TARGET_SPV_1_3,
				.code = shaderSource,
				.default_version = 100,
				.default_profile = GLSLANG_NO_PROFILE,
				.force_default_version_and_profile = false,
				.forward_compatible = false,
				.messages = GLSLANG_MSG_DEFAULT_BIT,
				.resource = glslang_default_resource(),
			};

		glslang_shader_t *shader = glslang_shader_create(&input);

		if (!glslang_shader_preprocess(shader, &input))
		{
			fprintf(stderr, "GLSL preprocessing failed\n");
			fprintf(stderr, "\n%s", glslang_shader_get_info_log(shader));
			fprintf(stderr, "\n%s", glslang_shader_get_info_debug_log(shader));
			printShaderSource(input.code);
			return 0;
		}

		if (!glslang_shader_parse(shader, &input))
		{
			fprintf(stderr, "GLSL parsing failed\n");
			fprintf(stderr, "\n%s", glslang_shader_get_info_log(shader));
			fprintf(stderr, "\n%s", glslang_shader_get_info_debug_log(shader));
			printShaderSource(glslang_shader_get_preprocessed_code(shader));
			return 0;
		}

		glslang_program_t *program = glslang_program_create();
		glslang_program_add_shader(program, shader);

		if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
		{
			fprintf(stderr, "GLSL linking failed\n");
			fprintf(stderr, "\n%s", glslang_program_get_info_log(program));
			fprintf(stderr, "\n%s", glslang_program_get_info_debug_log(program));
			return 0;
		}

		glslang_program_SPIRV_generate(program, stage);

		shaderModule.SPIRV.resize(glslang_program_SPIRV_get_size(program));
		glslang_program_SPIRV_get(program, shaderModule.SPIRV.data());

		{
			const char *spirv_messages =
				glslang_program_SPIRV_get_messages(program);

			if (spirv_messages)
				fprintf(stderr, "%s", spirv_messages);
		}

		glslang_program_delete(program);
		glslang_shader_delete(shader);

		return shaderModule.SPIRV.size();
	}

	VkResult createShaderModule(VkDevice device, ShaderModule *shader)
	{
		const VkShaderModuleCreateInfo createInfo =
			{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = shader->SPIRV.size() * sizeof(unsigned int),
				.pCode = shader->SPIRV.data(),
			};
		return vkCreateShaderModule(device, &createInfo, nullptr, &shader->shaderModule);
	}

	VkResult createShaderModuleFromSource(VkDevice device, ShaderModule *shader, const char *fileName)
	{
		if (compileShaderFile(fileName, *shader) < 1)
			return VK_NOT_READY;
		return createShaderModule(device, shader);
	}

	VkResult createShaderModuleFromSpv(VkDevice device, ShaderModule *shader, const char *spvfileName)
	{
		shader->SPIRV = readFile(spvfileName);
		return createShaderModule(device, shader);
	}

	size_t compileShaderFile(const char *file, ShaderModule &shaderModule)
	{
		if (auto shaderSource = readShaderFile(file); !shaderSource.empty())
			return compileShader(glslangShaderStageFromFileName(file), shaderSource.c_str(), shaderModule);

		return 0;
	}

	VkResult createGraphicsPipeline(VkDevice device)
	{
		ShaderModule vertShaderModule;
		ShaderModule fragShaderModule;

		if (createShaderModuleFromSpv(device, &vertShaderModule, "../data/shaders/full.vert.spv") != VK_SUCCESS)
		{
			return VK_NOT_READY;
		}
		if (createShaderModuleFromSpv(device, &fragShaderModule, "../data/shaders/full.frag.spv") != VK_SUCCESS)
		{
			return VK_NOT_READY;
		}
		// for vs
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule.shaderModule;
		vertShaderStageInfo.pName = "main";
		// for fs
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule.shaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		vkDestroyShaderModule(device, vertShaderModule.shaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule.shaderModule, nullptr);

		return VK_SUCCESS;
	}
}