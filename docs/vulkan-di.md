# ioc for vulkan

## High-level flow

    query instance validation layer: done    entity: validation layer

    query instance extension: done  entity: extensions

    createInstance(); done   entity: renderingContext

    setupDebugMessenger(); done  entity: debugger

    createSurface(); // entity surface from glfw: deps on instance + window 

    
    // physical devices maybe more than one, criteria, 
    // vksurface, computing and etc. 


    getPhysicalDevice();    entity: physical device, find suitable devide based on criteria. 

    physicalDevicePicker(); 

    createLogicalDevice();   entity: logical device, deps on physics picker + 

    getDeviceQueue(); done

    createSwapChain();  entity: swap chain, deps on surface + logic device, old swap chain must be referenced during resize

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
