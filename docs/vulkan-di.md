# ioc for vulkan

## High-level flow

    query instance validation layer: done    entity: validation layer

    query instance extension: done  entity: extensions

    createInstance(); done   entity: renderingContext

    setupDebugMessenger(); done  entity: debugger

    createSurface(); // entity surface from glfw: deps on instance + window 

    pickPhysicalDevice();    entity: physical device

    createLogicalDevice();   entity: logical device

    createSwapChain();  entity: swapchine 

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
