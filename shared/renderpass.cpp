#include <format>
#include <iostream>
#include <algorithm>
#include "renderpass.h"
#include "device.h"

using namespace std;
namespace SharedUtils
{
    VulkanRenderPass::VulkanRenderPass(std::shared_ptr<ILogicalDevice> device)
        : _device(device)
    {
        cout << format("--> VulkanRenderPass::VulkanRenderPass") << std::endl;


//   // finally, renderPass
//         VkRenderPassCreateInfo renderPassInfo = {
//             .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//             .pNext = nullptr,
//             .flags = 0,
//             .attachmentCount = 1,
//             .pAttachments = &colorAttachment,
//             .subpassCount = 1,
//             .pSubpasses = &subpass,
//             .dependencyCount = 1,
//             .pDependencies = &dependency};
//         VK_CHECK(vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &render_pass));



        cout << format("<-- VulkanRenderPass::VulkanRenderPass") << std::endl;
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
    }
}
