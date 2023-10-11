#include <format>
#include <iostream>
#include <algorithm>
#include "framebuffer.h"
#include "device.h"

using namespace std;
namespace SharedUtils
{
    VulkanFrameBuffer::VulkanFrameBuffer(std::shared_ptr<ILogicalDevice> device)
        : _device(device)
    {
        cout << format("--> VulkanFrameBuffer::VulkanFrameBuffer") << std::endl;
        cout << format("<-- VulkanFrameBuffer::VulkanFrameBuffer") << std::endl;
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
    }
}
