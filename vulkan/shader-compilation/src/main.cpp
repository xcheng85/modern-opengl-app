#define VK_NO_PROTOTYPES // volk
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define VOLK_IMPLEMENTATION
// #include "volk.h"

// following header include volk.sh and glslang_c_interface.h
#include "shared/vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "shared/di.hpp"

namespace di = boost::di;

using namespace SharedUtils;

void saveSPIRVBinaryFile(const char *filename, unsigned int *code, size_t size)
{
    FILE *f = fopen(filename, "w");

    if (!f)
        return;

    fwrite(code, sizeof(uint32_t), size, f);
    fclose(f);
}

void testShaderCompilation(const char *sourceFilename, const char *destFilename)
{
    ShaderModule shaderModule;

    if (compileShaderFile(sourceFilename, shaderModule) < 1)
        return;

    saveSPIRVBinaryFile(destFilename, shaderModule.SPIRV.data(), shaderModule.SPIRV.size());
}

// bool initVulkan()
// {
//     createInstance(&vk.instance);
// }

struct renderer
{
    int device;
};
class model
{
};

class iview
{
public:
    virtual ~iview() noexcept = default;
    virtual void update() = 0;
};

class gui_view : public iview
{
public:
    gui_view(std::string /*title*/, const renderer &r) { assert(42 == r.device); }
    void update() override {}
};

class text_view : public iview
{
public:
    void update() override {}
};
class controller
{
public:
    controller(model &, iview &) {}
};
class user
{
};

class app
{
public:
    app(controller &, user &) {}
};

int main()
{
    glslang_initialize_process();

    volkInitialize();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    // relative path between executable and data dir.
    testShaderCompilation("../data/shaders/full.vert", "full.vert.bin");
    testShaderCompilation("../data/shaders/full.frag", "full.frag.bin");

    // // vulkan init
    // VulkanInstance vk;
    // VulkanRenderDevice vkDev;

    // createGraphicsPipeline();
    auto injector = di::make_injector(
        di::bind<iview>().to<gui_view>(), di::bind<int>().to(42) // renderer device
    );

    injector.create<app>();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    glslang_finalize_process();

    return 0;
}