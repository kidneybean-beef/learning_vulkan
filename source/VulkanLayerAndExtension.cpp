#include "VulkanLayerAndExtension.h"
#include "VulkanApplication.h"

VulkanLayerAndExtension::VulkanLayerAndExtension()
{
    dbgCreateDebugReportCallback = NULL;
    dbgDestroyDebugReportCallback = NULL;
    debugReportCallback = NULL;
}

VulkanLayerAndExtension::~VulkanLayerAndExtension()
{
    dbgCreateDebugReportCallback = NULL;
    dbgDestroyDebugReportCallback = NULL;
    debugReportCallback = NULL;
}

/*
 * This function queries layer properties including extensions for all layers.
 * Resulting layer properties are stored in `layerPropertyList`.
 */
VkResult VulkanLayerAndExtension::getInstanceLayerProperties()
{
    uint32_t instanceLayerCount; // To store number of instance layers.
    std::vector<VkLayerProperties> layerProperties; // To store layer properties, Initially the size is 0.
    VkResult result; // Completion codes to check Vulkan API result status. VK_SUCCESS = 0

    // Query all the layers:
    do {
        // Get number of instance layer
        result = vkEnumerateInstanceLayerProperties(
            &instanceLayerCount,
            NULL); // All successful completion codes are non-negative values. All runtime error codes are negative values.

        if (result)
            return result;

        if (instanceLayerCount == 0)
            return VK_INCOMPLETE; // Return fail.

        layerProperties.resize(instanceLayerCount); // Now allocate memory by the
                                                    // obtained instanceLayerCount.
        result = vkEnumerateInstanceLayerProperties(
            &instanceLayerCount,
            layerProperties.data()); // Get layer properties for every layer,
                                     // each entry stores a VkLayerProperties for one layer
    } while (result == VK_INCOMPLETE); // While the returned vector is too small for the result

    // Query all the extensions for each layer and store it.
    std::cout << "\nInstanced Layers" << std::endl;
    std::cout << "===================" << std::endl;
    for (auto globalLayerProp : layerProperties) {
        // Print layer names and its description.
        std::cout << "\n"
                  << globalLayerProp.description << "\n\t|\n\t---[Layer Name]-->"
                  << globalLayerProp.layerName << "\n";

        LayerProperties layerProps;
        layerProps.properties = globalLayerProp;

        // Get instance level extensions for corresponding layer properties
        // resulting extensions are stored in layerProps.extensions.
        result = getExtensionProperties(layerProps);

        if (result) {
            continue; // If not successful but no error, leave this layer for other threads.
        }

        layerPropertyList.push_back(layerProps);

        // Print extension name for each instance layer.
        for (auto extension : layerProps.extensions) {
            std::cout << "\t\t|\n\t\t|---[Layer Extension]--> "
                      << extension.extensionName << "\n";
        }
    }
    return result;
}

/*
 * This function retrieves extension and its properties at instance and device level.
 * Pass a valid physical device pointer (gpu) to retrieve device level extensions,
 * otherwise use NULL to retrieve instance level extensions.
 * Resulting extensions are store 'in LayerProperties.extensions'.
 */
VkResult VulkanLayerAndExtension::getExtensionProperties(LayerProperties& layerProps,
    VkPhysicalDevice* gpu)
{
    uint32_t extensionCount; // Store number of extensions per layer.
    VkResult result; // Variable to check Vulkan API result status.
    char* layerName = layerProps.properties.layerName; // Name of the layer.

    do {
        // Get the total number of extension in this layer
        if (gpu) {
            result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, NULL);
        } else {
            result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, NULL);
        }

        if (result || extensionCount == 0)
            continue;

        layerProps.extensions.resize(extensionCount);

        // Gather all extension properties
        if (gpu) {
            result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, layerProps.extensions.data());
        } else {
            result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, layerProps.extensions.data());
        }
    } while (result == VK_INCOMPLETE);

    return result;
}

VkResult VulkanLayerAndExtension::getDeviceExtensionProperties(VkPhysicalDevice* gpu)
{
    VkResult result;

    // Query all the extensions for each layer and store it.
    std::cout << "Device extensions" << std::endl;
    std::cout << "==================" << std::endl;
    VulkanApplication* appObj = VulkanApplication::GetInstance();
    std::vector<LayerProperties>* instanceLayerProp = &appObj->GetInstance()->instanceObj.layerExtension.layerPropertyList;

    for (auto globalLayerProp : *instanceLayerProp) {
        LayerProperties layerProps;
        layerProps.properties = globalLayerProp.properties;

        if (result = getExtensionProperties(layerProps, gpu))
            continue;

        std::cout << "\n"
                  << globalLayerProp.properties.description << "\n\t|\n\t|---[Layer Name]--> " << globalLayerProp.properties.layerName << "\n";
        layerPropertyList.push_back(layerProps);

        if (layerProps.extensions.size()) {
            for (auto extension : layerProps.extensions) {
                std::cout << "\t\t|\n\t\t|---[Device Extension]--> " << extension.extensionName << "\n";
            }
        } else {
            std::cout << "\t\t|\n\t\t|---[Device Extension]--> No extension found \n";
        }
    }
    return result;
}

/*
 * Inspects the incoming layer names against system supported layers, if theses layers are not supported
 * then this function removed it from layerNames allowed
 */
VkBool32 VulkanLayerAndExtension::areLayersSupported(std::vector<const char*>& layerNames)
{
    uint32_t checkCount = (uint32_t)layerNames.size();
    uint32_t layerCount = (uint32_t)layerPropertyList.size();
    std::vector<const char*> unsupportedLayerNames;
    for (uint32_t i = 0; i < checkCount; i++) {
        VkBool32 isSupported = 0;
        for (uint32_t j = 0; j < layerCount; j++) {
            if (!strcmp(layerNames[i], layerPropertyList[j].properties.layerName)) {
                isSupported = 1;
            }
        }

        if (!isSupported) {
            std::cout << "No layer support found, remove layer: " << layerNames[i] << std::endl;
            unsupportedLayerNames.push_back(layerNames[i]);
        } else {
            std::cout << "Layer supported: " << layerNames[i] << std::endl;
        }
    }

    // **************************************************
    // why not deleting it right after found unsupported?
    // **************************************************
    for (auto i : unsupportedLayerNames) {
        auto it = std::find(layerNames.begin(), layerNames.end(), i);
        if (it != layerNames.end()) {
            layerNames.erase(it);
        }
    }

    return true;
}

/**
 * @brief 
 * @param msgFlags : The type of debugging event that has triggered the call. Eg, an error, warning, performance warning, etc.
 * @param objType : The type eobject that is manipulated by the triggering call.
 * @param srcObject : Handle of the object that's being created or manipulated by the triggered call.
 * @param location : Location of hte code describing the event.
 * @param msgCode : Message code.
 * @param layerPrefix : The layer responsible for triggering the debug event.
 * @param msg : The field constains the debug text.
 * @param userData : Any application-specific user data is specified to the callback using this field.
 * @return 
*/
VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanLayerAndExtension::debugFunction(VkFlags msgFlags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject, size_t location, int32_t msgCode,
    const char* layerPrefix, const char* msg, void* userData)
{
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] ERROR: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] WARNING: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] INFORMATION: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] PERFORMANCE: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] DEBUG: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else {
        return VK_FALSE;
    }

    fflush(stdout);
    return VK_TRUE;
}

VkResult VulkanLayerAndExtension::createDebugReportCallback()
{
    VkResult result;

    VulkanApplication* appObj = VulkanApplication::GetInstance();
    VkInstance* instance = &appObj->instanceObj.instance;

    // Get vkCreateDebugReportCallbackEXT API dynamically, which is not exposed statically.
    dbgCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugReportCallbackEXT");
    if (!dbgCreateDebugReportCallback) {
        std::cout << "Error: `GetInstanceProcAddr()' unable to locate `vkCreateDebugReportCallbackEXT` function." << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    std::cout << "`GetInstanceProcAddr()' loaded `dbgCreateDebugReportCallback` function.\n";

    // Get vkDestroyDebugReportCallbackEXT API
    dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkDestroyDebugReportCallbackEXT");
    if (!dbgDestroyDebugReportCallback) {
        std::cout << "Error: `GetInstanceProcAddr()` unable to locate `vkDestroyDebugReportCallbackEXT` function" << std::endl;
    }
    std::cout << "`GetInstanceProcAddr()` loaded `dbgDestroyDebugReportCallback` function\n";

    /*
     * Define the debug report control structure, provide the reference of 'debugFunction',
     * this function prints the debug information on the console.
     */
    dbgReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgReportCreateInfo.pfnCallback = debugFunction;
    dbgReportCreateInfo.pUserData = NULL;
    dbgReportCreateInfo.pNext = NULL;
    dbgReportCreateInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;

    // Create the debug report callback and store the handle into 'debugReportCallback', then function `debugReportCallback` is reday to use.
    result = dbgCreateDebugReportCallback(*instance, &dbgReportCreateInfo, NULL, &debugReportCallback);
    if (result == VK_SUCCESS) {
        std::cout << "Debug report callback object created successfully." << std::endl;
    }
    return result;
}

void VulkanLayerAndExtension::destroyDebugReportCallback()
{
    VulkanApplication* appObj = VulkanApplication::GetInstance();
    VkInstance& instance = appObj->instanceObj.instance;
    dbgDestroyDebugReportCallback(instance, debugReportCallback, NULL);
}
