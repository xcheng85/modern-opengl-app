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
    // framebuffer and render pass depends on this
    // this depends on imageview

    // owner is render frame for swapchain images
    // internal for custome rendeirng techinques: shadow etc. 

    class ImageAttachmentBinder
    {
    public:
        ImageAttachmentConnector();
        virtual ~ImageAttachmentConnector();
    };
}
