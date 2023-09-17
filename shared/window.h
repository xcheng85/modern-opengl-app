#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <volk.h>
#include <stdint.h>
#include <optional>
#include <string>
#include <vector>

using namespace std;

// forward declaration
struct GLFWwindow;

namespace SharedUtils
{
    class IWindow
    {
    public:
        virtual ~IWindow() noexcept = default;
        virtual bool ShouldClose() = 0;
    };

    class GlfwWindow : public IWindow
    {
    public:
        explicit GlfwWindow();
        virtual ~GlfwWindow();
        bool ShouldClose() override;

    private:
        GLFWwindow *_window = nullptr;
    };
}
