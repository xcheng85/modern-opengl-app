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
    class IInstance;
    class IWindow;
    class IRenderingSurface
    {
    public:
        virtual ~IRenderingSurface() noexcept = default;
        virtual std::any getSurface() = 0;
    };

    class VulkanRenderingSurface : public IRenderingSurface
    {
    public:
        explicit VulkanRenderingSurface(std::shared_ptr<IWindow>, std::shared_ptr<IInstance>);
        virtual ~VulkanRenderingSurface();
        inline std::any getSurface() override {
            return _surface;
        }
    private:
        VkSurfaceKHR _surface;
        std::shared_ptr<IInstance> _instance;
    };

}