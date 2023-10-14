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

    // deps
    // array of attachments

    // image, imageView + attachment become the render target

    //

    // Framebuffer(Device &device, const RenderTarget &render_target, const RenderPass &render_pass);

    // VkRenderPassBeginInfo: framebuffer and renderPass

    // array of subpasses
    // deps between subpasses

    class ILogicalDevice;
    class IRenderPass : public VulkanObject<VkRenderPass, VK_OBJECT_TYPE_RENDER_PASS>
    {
    public:
        virtual ~IRenderPass() noexcept = default;
    };

    enum class AttachmentType {
        Input,
        Color,
        ColorResolve,
        DepthStencil,
    };

    struct Attachment
    {
        VkFormat format{VK_FORMAT_UNDEFINED};
        VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
        VkImageLayout initialLayout{VK_IMAGE_LAYOUT_UNDEFINED};
        // from previous subpass
        VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // the end of current subpass
        VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    };

    struct Subpass
    {
        // index into vector<Attachment>
        std::vector<uint32_t> inputAttachmentIndex;
        std::vector<uint32_t> colorAttachmentIndex;
        std::vector<uint32_t> colorResolveAttachmentIndex;
        uint32_t depthStencilAttachmentIndex;
        // VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME supports multi-sample depth stencil
        // VkResolveModeFlagBits depth_stencil_resolve_mode;
    };

    class VulkanRenderPass : public IRenderPass
    {
    public:
        VulkanRenderPass(std::shared_ptr<ILogicalDevice>, const std::vector<Attachment> &, const std::vector<Subpass>&);
        virtual ~VulkanRenderPass();

    protected:
        

    private:
        std::shared_ptr<ILogicalDevice> _device;
    };
}