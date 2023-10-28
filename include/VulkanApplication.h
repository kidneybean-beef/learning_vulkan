#pragma once

#include "VulkanInstance.h"
#include "VulkanLayerAndExtension.h"
#include "VulkanDevice.h"

class VulkanApplication {
private:
    // Variables for singleton implementation.
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;
    bool debugFlag; // Whether to debug.

    VulkanApplication();

    // Wrapper function: create the Vulkan instance object
    VkResult createVulkanInstance(std::vector<const char*>& layers, std::vector<const char*>& extensions, const char* applicationName);
    VkResult handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char*>& layers, std::vector<const char*> &extensions);
    VkResult enumeratePhysicalDevice(std::vector<VkPhysicalDevice>& gpus);

public:
    VulkanInstance instanceObj;
    VulkanDevice* deviceObj;

    ~VulkanApplication();

    static VulkanApplication* GetInstance();

    // Simple program life cycle
    void initialize(); // Initialize and allocate resources
    void prepare(); // Prepare resource
    void update(); // Update data
    bool render(); // Render primitives
    void deInitialize(); // Release resources
};