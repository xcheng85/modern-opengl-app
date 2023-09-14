# ioc for vulkan

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

## create interface and dependency graph

IRenderingContext: 

vkInstance: validationLayers, device ext, appName, appVersion, engine name + version for game engine, 

wrapper on vkInstance: connection between you app and vulkan library, A Vulkan instance is similar, in concept, to an OpenGL rendering context.

IRenderingDebugger: 
  wrapper on debugger: vkIn

IRenderingSurface: 
wrapper on surface: vkIn, and osWindowHandle

IRenderingDevice
wrapper on vkRenderingDevice

OpenGL context is a Vulkan logical device (VkDevice). 
