#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <volk.h>
#include <stdint.h>
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

namespace SharedUtils
{
    class IWindow;
    class IApplication;
    class IPlatform
    {
    public:
        virtual ~IPlatform() = default;
        virtual void main_loop();

    protected:
        virtual void create_window() = 0;
        std::shared_ptr<IWindow> _window;
        std::shared_ptr<IApplication> _application;
    };

    class UnixPlatform : public IPlatform
    {
    public:
        explicit UnixPlatform(std::shared_ptr<IWindow>, std::shared_ptr<IApplication>);

    protected:
        virtual void create_window() override;
    };
};
