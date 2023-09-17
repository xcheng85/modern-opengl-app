#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <volk.h>
#include <stdint.h>
#include <optional>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace SharedUtils
{
    class IApplication
    {
    public:
        virtual ~IApplication() noexcept = default;
    };

    class Application : public IApplication
    {
    public:
        explicit Application();
    };
};
