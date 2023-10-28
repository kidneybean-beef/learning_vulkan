#include "VulkanInstance.h"

VkResult VulkanInstance::createInstance(std::vector<const char*>& layers, std::vector<const char*>& extensionNames, char const* const appName)
{
    layerExtension.appRequestedExtensionNames = extensionNames;
    layerExtension.appRequestedLayerNames = layers;

    // Define the Vulkan application structure
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = appName;
    appInfo.engineVersion = 1;
    // VK_API_VERSION is now deprecated, use VK_MAKE_VERSION instead.
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    // Define the Vulkan instance create info structure
    VkInstanceCreateInfo instCreateInfo = {};
    instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //instCreateInfo.pNext = VK_NULL_HANDLE;
    instCreateInfo.pNext = &layerExtension.dbgReportCreateInfo; // In order to enable debugging.
    instCreateInfo.flags = 0;
    instCreateInfo.pApplicationInfo = &appInfo;

    // Specify the list of layer name to be enabled.
    instCreateInfo.enabledLayerCount = (uint32_t)layers.size();
    instCreateInfo.ppEnabledLayerNames = layers.size() ? layers.data() : NULL;

    // Specify the list of extensions to be used in the application.
    instCreateInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
    instCreateInfo.ppEnabledExtensionNames = extensionNames.size() ? extensionNames.data() : NULL;

    VkResult result = vkCreateInstance(&instCreateInfo, NULL, &instance);
    std::cout << "'vkCreateInstance' return = " << result << std::endl;
    assert(result == VK_SUCCESS);

    return result;
}

void VulkanInstance::destroyInstance()
{
    vkDestroyInstance(instance, NULL);
}