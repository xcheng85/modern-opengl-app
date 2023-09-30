#include <format>
#include <iostream>
#include <algorithm>
#include "surface.h"
#include "window.h"
#include "instance.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace std;
namespace SharedUtils
{
    VulkanRenderingSurface::VulkanRenderingSurface(
        std::shared_ptr<IWindow> window,
        std::shared_ptr<IInstance> instance)
    {
        // detect window type based on rtti: typeid operator
        cout << format("--> VulkanRenderingSurface::VulkanRenderingSurface {}", typeid(*window).name()) << std::endl;
        auto const &handle = window->getWindow();
        if (handle.has_value())
        {
            auto const &type = handle.type();
            auto const &typeName = string{type.name()};
            // substring member function version
            if (typeName.find("GLFWwindow") != typeName.npos)
            {
                auto glfw = std::any_cast<GLFWwindow *>(handle);
                auto const &vkInstance = any_cast<VkInstance>(instance->getInstance());
                VkResult errCode = glfwCreateWindowSurface(vkInstance, glfw, NULL, &_surface);
                if (errCode != VK_SUCCESS)
                {
                    throw runtime_error("falied to create windows surface");
                }
            }
        }
        cout << format("<-- VulkanRenderingSurface::VulkanRenderingSurface") << std::endl;
    }

    VulkanRenderingSurface::~VulkanRenderingSurface()
    {
    }
}
