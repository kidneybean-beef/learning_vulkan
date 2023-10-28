#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanApplication.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice* physicalDevice)
{
    gpu = physicalDevice;
}

VulkanDevice::~VulkanDevice()
{
}

// NOTE: This function requires queue object to be in existance before.
// By VulkanDevice::getGraphicsHanle()
VkResult VulkanDevice::createDevice(std::vector<const char*>& layers, std::vector<const char*>& extensions)
{
    layerExtension.appRequestedExtensionNames = layers;
    layerExtension.appRequestedExtensionNames = extensions;

    VkResult result;
    float queuePriorities[1] = { 0.0 };
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = NULL;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = NULL; // Device layers are deprecated.
    deviceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : NULL;
    deviceCreateInfo.pEnabledFeatures = NULL;

    result = vkCreateDevice(*gpu, &deviceCreateInfo, NULL, &device);
    assert(result == VK_SUCCESS);

    return result;
}

void VulkanDevice::destroyDevice()
{
    vkDestroyDevice(device, NULL);
}

void VulkanDevice::getPhysicalDeviceQueuesAndProperties()
{
    // Query queue families count with pass NULL as second parameter.
    vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, NULL);

    // Allocate memory to accomodate queue properties.
    queueFamilyProps.resize(queueFamilyCount);

    // Get queue family properties
    vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, queueFamilyProps.data());
}

/*
 * This function stores the handle of the graphics queue in `graphicsQueueIndex`, which is used
 * in the creation of the logical device (VkDevice) object.
 */
uint32_t VulkanDevice::getGraphicsQueueHandle()
{
    bool found = false;
    // 1. Iterate the number of queues supported by the physical device.
    for (unsigned int i = 0; i < queueFamilyCount; i++) {
        // Get the graphics queue type.
        if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // Get the handle/index ID of the graphics queue family.
            found = true;
            graphicsQueueIndex = i;
            break;
        }
    }

    // Assert if there is no queue found.
    assert(found);

    return 0;
}

/*
* High level wrapper function to get the device's associated queue.
*/
void VulkanDevice::getDeviceQueue()
{
    vkGetDeviceQueue(device, graphicsQueueWithPresentIndex, 0, &queue);
}