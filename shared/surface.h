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
    // ioc
    class IInstance;
    class IWindow;
    class IRenderingSurface : public VulkanObject<VkSurfaceKHR, VK_OBJECT_TYPE_SURFACE_KHR>
    {
    public:
        virtual ~IRenderingSurface() noexcept = default;
    };

    class VulkanRenderingSurface : public IRenderingSurface
    {
    public:
        explicit VulkanRenderingSurface(std::shared_ptr<IWindow>, std::shared_ptr<IInstance>);
        virtual ~VulkanRenderingSurface();
    private:
        std::shared_ptr<IInstance> _instance;
    };

}