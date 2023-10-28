// This provides layer- and extension-based functionalities for the instance and
// device. It also offers debugging capabilities.

#pragma once

#include "Headers.h"

struct LayerProperties {
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtension {
public:
    VulkanLayerAndExtension();
    ~VulkanLayerAndExtension();

    /******* LAYER_AND_EXTENSION VARAIBLES AND MEMBER FUNCTION *******/

    // List of layer names requested by the application.
    std::vector<const char*> appRequestedLayerNames;

    // List of extension names requested by the application.
    std::vector<const char*> appRequestedExtensionNames;

    // Layers and corresponding extension list.

    std::vector<LayerProperties> layerPropertyList; // system supported layer property info.

    // Instance/global layer
    VkResult getInstanceLayerProperties(); // This helper function will query either
                                           // instance or global layers. It gets the total
                                           // count of the layers and stores all of the
                                           // layer information in a VkLayerProperties

    // Global extensions
    VkResult getExtensionProperties(LayerProperties& layerProps,
        VkPhysicalDevice* gpu = NULL);

    // Device-based extensions
    VkResult getDeviceExtensionProperties(VkPhysicalDevice* gpu);

    /******* VULKAN DEBUGGING MEMBER FUNCTION AND VARAIBLES *******/

    // This function inspects the incoming layer names against system-supported layers.
    VkBool32 areLayersSupported(std::vector<const char*>& layerNames);
    VkResult createDebugReportCallback();
    void destroyDebugReportCallback();

    // This user-defined funciton prints the retrieved bug infos in a user-friendly way.
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject,
        size_t location,
        int32_t msgCode,
        const char* layerPrefix,
        const char* msg,
        void* userData);

private:
    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback; // This is a funcion pointer used to create 'debugReportCallback'.
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
    VkDebugReportCallbackEXT debugReportCallback;

public:
    VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {}; // Defines the behaviour of the debugging: what infos should be included.
};