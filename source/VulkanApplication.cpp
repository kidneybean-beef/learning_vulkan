#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::instance;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char*> layerNames;
extern std::vector<const char*> instanceExtensionNames;
extern std::vector<const char*> deviceExtensionNames;

// Application constructor for layer enumeration.
VulkanApplication::VulkanApplication()
{
    // At application start up, enumerate instance layers.
    instanceObj.layerExtension.getInstanceLayerProperties();

    deviceObj = NULL;
    debugFlag = true;
}

VkResult VulkanApplication::createVulkanInstance(std::vector<const char*>& layers,
    std::vector<const char*>& extensions,
    const char* appName)
{
    return instanceObj.createInstance(layers, extensions, appName);
}

/*
 * This function is responsible for creating the logical device.
 * The creation requires the following steps:
 * 1. Get the physical device specific layer and corresponding extensions
 * 2. Create user defined `VulkanDevice` object
 * 3. Provide the list of layer and extension that needs to enabled in this physical device
 * 4. Get the physical device/GPU properties
 * 5. Get the memory properties from the physical device/GPU
 * 6. Query the physical device exposed queues and related properties
 * 7. Get the handle of graphics queue
 * 8. Create the logical device, connect it to the graphics queue
 */
VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char*>& layers, std::vector<const char*>& extensions)
{
    deviceObj = new VulkanDevice(gpu);
    if (!deviceObj) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Print the devices available layer and their extension.
    deviceObj->layerExtension.getDeviceExtensionProperties(gpu);

    // Get the physical device/GPU properties.
    vkGetPhysicalDeviceProperties(*gpu, &deviceObj->gpuProps);

    // Get the memory properties from the physical device.GPU.
    vkGetPhysicalDeviceMemoryProperties(*gpu, &deviceObj->memoryProperties);

    // Query the available queues on the physical device and their properties.
    deviceObj->getPhysicalDeviceQueuesAndProperties();

    // Retrieve the queue which support graphics pipeline.
    deviceObj->getGraphicsQueueHandle();

    // Create logical device, ensure that this device is conneced to graphics queue.
    return deviceObj->createDevice(layers, extensions);
}

VkResult VulkanApplication::enumeratePhysicalDevice(
    std::vector<VkPhysicalDevice>& gpuList)
{
    // Hold the gpu count.
    uint32_t gpuDeviceCount;

    // Get the gpu count.
    VkResult result = vkEnumeratePhysicalDevices(instanceObj.instance, &gpuDeviceCount, NULL);
    assert(result == VK_SUCCESS);

    assert(gpuDeviceCount);

    // Make space for restrieval
    gpuList.resize(gpuDeviceCount);

    // Get physical device object
    result = vkEnumeratePhysicalDevices(instanceObj.instance, &gpuDeviceCount, gpuList.data());
    assert(result == VK_SUCCESS);

    return result;
}

VulkanApplication::~VulkanApplication() { }

VulkanApplication* VulkanApplication::GetInstance()
{
    std::call_once(onlyOnce, []() { instance.reset(new VulkanApplication()); });
    return instance.get();
}

void VulkanApplication::initialize()
{
    char title[] = "Hello World!!!";

    // Check if the supplied layer are supported or not such that typos could be detected.
    if (debugFlag) {
        instanceObj.layerExtension.areLayersSupported(layerNames);
    }

    // Create the Vulkan instance wit specified layer and extension names.
    createVulkanInstance(layerNames, instanceExtensionNames, title);

    // Create the debugging report if debugging is enabled
    if (debugFlag) {
        instanceObj.layerExtension.createDebugReportCallback();
    }

    // Get the list of physical devices on the system.
    std::vector<VkPhysicalDevice> gpuList;
    enumeratePhysicalDevice(gpuList);

    // Assuming the system has only one GPU.
    if (gpuList.size() > 0) {
        handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);
    }
}

void VulkanApplication::prepare()
{
}

void VulkanApplication::update()
{
}

bool VulkanApplication::render()
{
    return true;
}

void VulkanApplication::deInitialize()
{
    deviceObj->destroyDevice();
    if (debugFlag) {
        instanceObj.layerExtension.destroyDebugReportCallback();
    }
    instanceObj.destroyInstance();
}