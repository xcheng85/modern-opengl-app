#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <volk.h>
#include <stdint.h>
#include <optional>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// forward declaration
struct GLFWwindow;

namespace SharedUtils
{
    class IPlatform;
    class IWindow
    {
    public:
        virtual ~IWindow() noexcept = default;
        virtual bool ShouldClose() = 0;
        virtual void ProcessEvents() = 0;
        virtual void Close() = 0;
        virtual void usePlatform(const IPlatform* const);

    protected:
        IPlatform* _platform;
    };

    class GlfwWindow : public IWindow
    {
    public:
        // avoid circular deps
        explicit GlfwWindow();
        virtual ~GlfwWindow();
        bool ShouldClose() override;
        void ProcessEvents() override;
        void Close() override;
        void usePlatform(const IPlatform* const p) override;

    private:
        GLFWwindow *_window = nullptr;
    };
}
