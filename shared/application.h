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

        virtual void init() = 0;

        virtual void update() = 0;

        virtual void resize(const uint32_t width, const uint32_t height) = 0;

        virtual void terminate() = 0;
    };

    class Application : public IApplication
    {
    public:
        explicit Application();
        virtual ~Application();

        void init() override;
        void update() override;
        void resize(const uint32_t width, const uint32_t height) override;
        void terminate() override; 
    };
};