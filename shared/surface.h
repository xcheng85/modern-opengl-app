#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <volk.h>
#include "di.hpp"
#include "utils.h"

namespace SharedUtils
{
    // ioc
    class IRenderingContext;
    class IWindow;
    class IRenderingSurface
    {
    public:
        virtual ~IRenderingSurface() noexcept = default;
    };

    class VulkanRenderingSurface : public IRenderingSurface
    {
    public:
        explicit VulkanRenderingSurface(std::shared_ptr<IWindow>, std::shared_ptr<IRenderingContext>);
        virtual ~VulkanRenderingSurface();
    private:
        VkSurfaceKHR _surface;
    };

}