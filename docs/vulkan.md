# Vulkan

## High-level flow

    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();

## vulkan instance

## validation layer
optional
simply enable validation layers for debug builds and completely disable them for release builds, 

LunarG validation layers

instance validation layers apply to all Vulkan calls

All of the useful standard validation is bundled into a layer included in the SDK that is known as VK_LAYER_KHRONOS_validation

The validation layers will print debug messages to the standard output by default, but we can also handle them ourselves by providing an explicit callback in our program

## vulkan physical device
gpu
vkPhysicalDevice

1. enumerate physical device

device type: discrete and integrate gpu

device supports multiple command queue family:
1. computer
2. memory transfer
3. graphics: draw
4. present 

each family can support multiple command queues;



## device extension
device specific extension is VK_KHR_swapchain: for rendering, WSI (Window System Integration) extensions
other for computing

## physical device feature
geometry shaders

## logical device
multiple logical devices from the same physical device 

select the feature from physical device for the logical device: geometry shaders

the number of queues we want for a single queue family

assign priorities to queues

don't really need more than one

create all of the command buffers on multiple threads and then submit them all at once on the main thread with a single low-overhead call.

vkCreateDevice: no physical -> logic device

## command queues for logic device
Device queues are implicitly cleaned up when the device is destroyed,

retrieve queue handles for each queue family

graphics queue and presentation queue

logical device and queue handles == graphics

presentation queue handles


queue families supporting drawing commands and the ones supporting presentation do not overlap
prefer a physical device that supports drawing and presentation in the same queue for improved performance.



## windows surface (needs vk instancd)
presentation is a queue-specific feature

VkSurfaceKHR

optional: for offscreen rendering
opengl: hack for create invisible window

creating surface is dependent on the platform: 

VK_KHR_xcb_surface for linux: GLFW actually has glfwCreateWindowSurface that handles the platform differences for us

GLFW window pointer

we still need to destroy vk surface by us, not GLFW

## Swap chain

4 steps:
1. query cap
2. choose surface format
3. choose presentation mode
4. choose swap extent


created explicitly in Vulkan.
implicitly in OpenGL.

The swap chain is essentially a queue of images 

application will acquire such an image to draw to it

synchronize the presentation of images with the refresh rate of the screen.

not in the vulkan core: 

VK_KHR_swapchain device extension is not available always, check

check swapchain device  compatible with our window surface: 4k 
vkGetPhysicalDeviceSurfaceCapabilitiesKHR: vkDevice + surface handle

Using a swapchain requires enabling the VK_KHR_swapchain extension first.

surface format: SRGB 8, 
colorspace: SRGB_NONLINEAR

presentation mode: VK_PRESENT_MODE_FIFO_KHR 
VK_PRESENT_MODE_MAILBOX_KHR: triple, high energy
mobile: VK_PRESENT_MODE_FIFO_KHR

high DPI display (like Apple's Retina display), screen coordinates don't correspond to pixels.

imageArrayLayers > 1: for VR

VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: direct rendering

POST-PROCESSING: VK_IMAGE_USAGE_TRANSFER_DST_BIT


f the graphics queue family and presentation queue family are the same, which will be the case on most hardware, then we should stick to exclusive mode

## Retrieving images from swap chain

number of images in swap chain is configurable

handles of the VkImage

these images that can be drawn onto and can be presented to the window

## image view to image
SQL db

you can treat 2d image as 1d, vice versa

purpose: color or depth

vr: multiple views on the same image: left and right eye view

image view: for texture, but not for render target. 
English / Français
Introduction
Overview
Development environment
 Drawing a triangle
 Vertex buffers
 Uniform buffers
 Texture mapping
Depth buffering
Loading models
Generating Mipmaps
Multisampling
Compute Shader
FAQ
Privacy policy
GitHub Repository
Support the website

Vulkan Specification
LunarG Vulkan SDK
Vulkan Guide
Vulkan Hardware Database

Rust code
Java code
Go code
Visual Studio 2019 samples
This site is not affiliated with or endorsed by the Khronos Group. Vulkan® and the Vulkan logo are trademarks of the Khronos Group Inc.
Compute Shader

color blending: 

 
If you are only interested in depth values then you can disable the fragment shader stage

## vertex shader
gl_VertexIndex
gl_Position: ndc
clip coordinate and NDC
NDC: -1, 1, z: 0,1

## fragment shader
framebuffer #0
layout(location = 0)

shader modules: one stage of shader

## render pass: framebuffer attachments + subpasses

framebuffer attachments
multi-sampling
how many color and depth buffers


intial image layout

render pass

final image layout: 1. for presenting in the swap chain . 2. for other purpose

A single render pass can consist of multiple subpasses: for post-processing
Subpasses are subsequent rendering operations that depend on the contents of framebuffers in previous passes

subpass and referenceing of attachment

Subpass dependencies: 


## framebuffer (swap chain image view + render pass)
only use a framebuffer with the render passes that it is compatible with, which roughly means that they use the same number and type of attachments.


##  Command buffers
drawing operations and memory transfers,

record all of the operations you want to perform in command buffer objects

command recording to happen in multiple threads

## Command pool
memory support for command buffer

command pool created in device queue: graphics or presentation

## create and recording command buffer
recording
for 1 render pass:

vkBeginCommandBuffer

vkCmdBeginRenderPass
vkCmdBindPipeline
vkCmdDraw
vkCmdEndRenderPass

vkEndCommandBuffer

## flow of a single frame
Wait for the previous frame to finish

Acquire an image from the swap chain

Record a command buffer which draws the scene onto that image

Submit the recorded command buffer

Present the swap chain image

this flow requires order preservation: 

GPU : semophore
CPU: fence

semophore for swap chain order
fence for drawFrame on cpu

## synchornization on GPU: swapchain 

1. acquire image to render from swap chain, done, semophore signal
2. wait for semophore to signal , then recording command buffer
3. command buffer finished execution on GPU, signal sempohre
4. presentation wait for the semophore then present: vkQueuePresentKHR

## triple frames
round-robin three frames to avoid sync

## window resize and swap chain

resize

minimum window, 0,0

## vulkan debug utilities
VkDebugUtilsMessengerEXT
VkDebugReportCallbackEXT

VkDebugUtilsObjectNameInfoEXT: handle --> name. 

associate with vk instance

VulkanDebugCallback

## rendering loop
semaphore and sync

vulkan api: async

mechnisum of correct order:

semaphore
fence


## complile Shader offline
glslangValidator
glslc: support #includes 
https://github.com/google/shaderc/tree/main/glslc#source-filename-based-line-and-__file__
unit test: https://github.com/google/shaderc/blob/main/glslc/test/include.py


```shell
which glslc
/usr/bin/glslc
glslc ./data/shaders/full.vert -o full.spv
```

## Compile Shader on the runtime

glslang: compiler

SPIR-V: binary

cmake static link to glslang lib

GLSL, HLSL

Serve client: vulkan and opengl

version: vulkan 1.x and opengl 4.5

profile: core, compatible and ES (mobile)


link shader to program
generate binary to file

## load shader from compile bin


## 