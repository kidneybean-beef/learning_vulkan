/*
 * This class create/destruct the Vulkan instance objectand is helpful during initialization.
 * A vulkan instance is a primary object that is required to build an application; It
 * stores all the application states. It is of the type VkInstance and is managed inside
 * the VulkanInstance class, which is user-defined.(VulkanInstance.h/cpp)
 */

#pragma once

#include "VulkanLayerAndExtension.h"

class VulkanInstance {
public:
    VkInstance instance;
    VulkanLayerAndExtension layerExtension; // Vulkan instance specific layer and extensions

public:
    VulkanInstance() { }
    ~VulkanInstance() { }

public:
    VkResult createInstance(std::vector<const char*>& layers, std::vector<const char*> &extensions, const char* applicationName);
    void destroyInstance();
};