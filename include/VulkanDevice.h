// This is responsible for creating the logical and physical devices.
// Each physical device is capable of exposing one or more queues.
// This class also manages a device's queue and it respective properties.

#pragma once

#include "Headers.h"
#include "VulkanLayerAndExtension.h"

class VulkanDevice {
public:
    VkDevice device; // Logical device
    VkPhysicalDevice* gpu; // Physical device
    VkPhysicalDeviceProperties gpuProps; // Physical device atributes
    VkPhysicalDeviceMemoryProperties memoryProperties;

    VkQueue queue;
    std::vector<VkQueueFamilyProperties> queueFamilyProps; // Each VkQueueFamilyProperties constains a queueFLag
                                                           // indicates the queue type.
    uint32_t graphicsQueueIndex;
    uint32_t graphicsQueueWithPresentIndex;
    uint32_t queueFamilyCount;

    VulkanLayerAndExtension layerExtension;

    VulkanDevice(VkPhysicalDevice* gpu);
    ~VulkanDevice();

public:
    VkResult createDevice(std::vector<const char*>& layers,
        std::vector<const char*>& extensions);
    void destroyDevice();

    // Get the available queues exposed by the physical devices
    void getPhysicalDeviceQueuesAndProperties();

    // Query physical device to retrive queue properties
    uint32_t getGraphicsQueueHandle();

    void getDeviceQueue();
};