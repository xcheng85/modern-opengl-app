#include <malloc.h>
#include <string.h>
#include <string>
#include <cassert>
#include <numeric>
#include <vector>

#include "utils.h"
#include "vulkan.h"
namespace SharedUtils
{
	void CHECK(bool check, const char *fileName, int lineNumber)
	{
		if (!check)
		{
			printf("CHECK() failed at %s:%i\n", fileName, lineNumber);
			assert(false);
			exit(EXIT_FAILURE);
		}
	}

	VulkanRenderingContextValidationLayers::VulkanRenderingContextValidationLayers(const std::vector<std::string> &validationLayers)
		: _validationLayers(validationLayers)
	{
		//assert(validationLayers.size() == 1);
	}

	VulkanRenderingContextExtensions::VulkanRenderingContextExtensions(const std::vector<std::string> &extensions)
		: _extensions(extensions)
	{
		//assert(extensions.size() == 4);
	}

	VulkanRenderingHostAppSettings::VulkanRenderingHostAppSettings(const std::string name, const std::string version)
		: _name(name), _version(version)
	{
	}

	VulkanRenderingContext::VulkanRenderingContext(IRenderingContextValidationLayer &validationLayers, IRenderingContextExtensions &extensions, IRenderingHostAppSettings &hostAppSettings)
	{
		const VkApplicationInfo appinfo =
			{
				.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.pNext = nullptr,
				.pApplicationName = hostAppSettings.name().c_str(),
				.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
				.pEngineName = "No Engine",
				.engineVersion = VK_MAKE_VERSION(1, 0, 0),
				.apiVersion = VK_API_VERSION_1_3};

		const std::vector<std::string> t = validationLayers.getValidationLayers();
		const std::vector<std::string> e = extensions.getExtensions();
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

		std::vector<const char *> layer_names(validationLayers.getValidationLayers().size());
		std::transform(validationLayers.getValidationLayers().begin(), validationLayers.getValidationLayers().end(), layer_names.begin(), [](const std::string &s)
					   { return s.c_str(); });
		std::vector<const char *> extensitions_names(extensions.getExtensions().size());
		std::transform(extensions.getExtensions().begin(), extensions.getExtensions().end(), extensitions_names.begin(), [](const std::string &s)
					   { return s.c_str(); });

		const VkInstanceCreateInfo createInfo =
			{
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.pApplicationInfo = &appinfo,
				.enabledLayerCount = static_cast<uint32_t>(validationLayers.getValidationLayers().size()),
				.ppEnabledLayerNames = layer_names.data(),
				.enabledExtensionCount = static_cast<uint32_t>(extensions.getExtensions().size()),
				.ppEnabledExtensionNames = extensitions_names.data()};

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &_instance));
		// This function will load all required Vulkan entrypoints, including all extensions; you can use Vulkan from here on as usual.
		volkLoadInstance(_instance);
	}

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