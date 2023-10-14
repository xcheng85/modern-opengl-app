#include <format>
#include <iostream>
#include <algorithm>
#include <ranges>

#include "renderpass.h"
#include "device.h"

using namespace std;
using namespace VulkanUtils;
namespace rg = std::ranges;
namespace SharedUtils
{
    VulkanRenderPass::VulkanRenderPass(std::shared_ptr<ILogicalDevice> device,
                                       const std::vector<Attachment> &attachments,
                                       const std::vector<Subpass> &subpasses)
        : _device(device)
    {
        cout << format("--> VulkanRenderPass::VulkanRenderPass") << std::endl;
        // 1. attachments
        // lamda function for create all the attachment descriptions
        auto createVkAttachmentDescriptions = [&attachments]()
        {
            std::vector<VkAttachmentDescription> descs;

            for (size_t i = 0U; i < attachments.size(); ++i)
            {
                const auto &[colorFormat, samples, initialLayout, loadOp, storeOp, stencilLoadOp, stencilStoreOp] = attachments[i];
                VkAttachmentDescription attachment{
                    .flags = 0,
                    .format = colorFormat,
                    .samples = samples,
                    .loadOp = loadOp,
                    .storeOp = storeOp,
                    .stencilLoadOp = stencilLoadOp,
                    .stencilStoreOp = stencilStoreOp,
                    .initialLayout = initialLayout,
                    // this assumes the attachment is for intemediate rendering
                    .finalLayout = (isDepthFormat(colorFormat) || isDepthStencilFormat(colorFormat)) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                                                                                     : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                };
                descs.push_back(std::move(attachment));
            }
            return descs;
        };
        auto vkAttachmentDescriptions = createVkAttachmentDescriptions();

        // 2. subpass
        auto transformVkAttachmentReference = [&attachments](const std::vector<unsigned int> attachmentIndex,
                                                             AttachmentType attachmentType)
        {
            // fp with lamda function
            std::vector<VkAttachmentReference> attachmentRefs(attachmentIndex.size());
            auto v = attachmentIndex | rv::transform(
                                           [&attachments, attachmentType](const unsigned int &idx)
                                           {
                                               const auto &initialLayout = attachments[idx].initialLayout;
                                               const auto &colorFormat = attachments[idx].format;
                                               VkImageLayout defaultImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                               switch (attachmentType)
                                               {
                                                   using enum AttachmentType;
                                               case Input:
                                                   defaultImageLayout = (isDepthFormat(colorFormat) || isDepthStencilFormat(colorFormat))
                                                                            ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                                                                            : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                                   break;
                                               case Color:
                                                   defaultImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                                                   break;
                                               case ColorResolve:
                                                   defaultImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                                                   break;
                                               }
                                               VkAttachmentReference attachmentRef{idx, initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? defaultImageLayout : initialLayout};
                                               return attachmentRef;
                                           });
            rg::copy(v, attachmentRefs.begin());
            return attachmentRefs;
        };
        std::vector<VkSubpassDescription> vkSubpassDescriptions;
        for (size_t i = 0; i < subpasses.size(); ++i)
        {
            const auto &[inputAttachmentIndex,
                         colorAttachmentIndex,
                         colorResolveAttachmentIndex,
                         depthStencilAttachmentIndex] = subpasses[i];
            using enum AttachmentType;
            const auto &inputAttachments = transformVkAttachmentReference(inputAttachmentIndex, Input);
            const auto &colorAttachments = transformVkAttachmentReference(colorAttachmentIndex, Color);
            const auto &colorResolveAttachments = transformVkAttachmentReference(colorResolveAttachmentIndex, ColorResolve);
            const auto &depthStencilAttachments = transformVkAttachmentReference({depthStencilAttachmentIndex}, DepthStencil);

            VkSubpassDescription desc{
                .flags = 0,
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .inputAttachmentCount = inputAttachments.size(),
                .pInputAttachments = inputAttachments.size() == 0 ? nullptr : inputAttachments.data(),
                .colorAttachmentCount = colorAttachments.size(),
                .pColorAttachments = colorAttachments.size() == 0 ? nullptr : colorAttachments.data(),
                .pResolveAttachments = colorResolveAttachments.size() == 0 ? nullptr : colorResolveAttachments.data(),
                .pDepthStencilAttachment = depthStencilAttachments.size() == 0 ? nullptr : depthStencilAttachments.data(),
            };
            // to do depth_resolve
            // avoid a copy
            vkSubpassDescriptions.emplace_back(std::move(desc));
        }

        // 3. adjust attachment initial and final layout based on the subpass
        auto adjustAttachmentLayout = [&vkAttachmentDescriptions, &vkSubpassDescriptions]()
        {
            // adjust initial layout
            // an attachmetment may exist in subpass 0, 1, etc.
            for (auto &subpass : vkSubpassDescriptions)
            {
                for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
                {
                    auto &reference = subpass.pColorAttachments[k];
                    if (vkAttachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                    {
                        vkAttachmentDescriptions[reference.attachment].initialLayout = reference.layout;
                    }
                }

                for (size_t k = 0U; k < subpass.inputAttachmentCount; ++k)
                {
                    auto &reference = subpass.pInputAttachments[k];
                    if (vkAttachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                    {
                        vkAttachmentDescriptions[reference.attachment].initialLayout = reference.layout;
                    }
                }

                if (subpass.pDepthStencilAttachment)
                {
                    auto &reference = *subpass.pDepthStencilAttachment;
                    if (vkAttachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                    {
                        vkAttachmentDescriptions[reference.attachment].initialLayout = reference.layout;
                    }
                }

                if (subpass.pResolveAttachments)
                {
                    for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
                    {
                        auto &reference = subpass.pResolveAttachments[k];
                        if (vkAttachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                        {
                            vkAttachmentDescriptions[reference.attachment].initialLayout = reference.layout;
                        }
                    }
                }
                // to multi-sample depth stencil
            }
            // adjust final layout
            // only care about the last subpass
            auto &lastSubpass = vkSubpassDescriptions.back();

            for (size_t k = 0U; k < lastSubpass.colorAttachmentCount; ++k)
            {
                const auto &reference = lastSubpass.pColorAttachments[k];

                vkAttachmentDescriptions[reference.attachment].finalLayout = reference.layout;
            }

            for (size_t k = 0U; k < lastSubpass.inputAttachmentCount; ++k)
            {
                const auto &reference = lastSubpass.pInputAttachments[k];

                vkAttachmentDescriptions[reference.attachment].finalLayout = reference.layout;

                const auto colorFormat = vkAttachmentDescriptions[reference.attachment].format;

                // if a depth stencil attachment is a shader input,
                // do not use it as depth stencil ?
                if (isDepthFormat(colorFormat) || isDepthStencilFormat(colorFormat))
                {
                    lastSubpass.pDepthStencilAttachment = nullptr;
                }
            }

            if (lastSubpass.pDepthStencilAttachment)
            {
                const auto &reference = *lastSubpass.pDepthStencilAttachment;

                vkAttachmentDescriptions[reference.attachment].finalLayout = reference.layout;
            }

            if (lastSubpass.pResolveAttachments)
            {
                for (size_t k = 0U; k < lastSubpass.colorAttachmentCount; ++k)
                {
                    const auto &reference = lastSubpass.pResolveAttachments[k];

                    vkAttachmentDescriptions[reference.attachment].finalLayout = reference.layout;
                }
            }
            // to do: multiple sample depth stencil
        };

        adjustAttachmentLayout();

        // 4. dependencies between subpasses
        // subpass at least 1
        const auto subPassCount = std::max<size_t>(1, subpasses.size());
        std::vector<VkSubpassDependency> dependencies(subPassCount - 1);

        if (subPassCount > 1)
        {
            for (size_t i = 0; i < dependencies.size(); ++i)
            {
                dependencies[i].srcSubpass = i;
                dependencies[i].dstSubpass = i + 1;
                dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
            // it does not include: VK_SUBPASS_EXTERNAL
        }
        // 5. create renderPass
        VkRenderPassCreateInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = vkAttachmentDescriptions.size(),
            .pAttachments = vkAttachmentDescriptions.data(),
            .subpassCount = vkSubpassDescriptions.size(),
            .pSubpasses = vkSubpassDescriptions.data(),
            .dependencyCount = dependencies.size(),
            .pDependencies = dependencies.data()};
        auto vkDevice = device->getVkHandle();
        VK_CHECK(vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &_handle));
        cout << format("<-- VulkanRenderPass::VulkanRenderPass") << std::endl;
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        auto vkDevice = _device->getVkHandle();
        vkDestroyRenderPass(vkDevice, _handle, nullptr);
    }
}
