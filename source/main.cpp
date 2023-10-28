// helloWorld.cpp : Defines the entry point for the application.
//

#include "Headers.h"
#include "VulkanApplication.h"

std::vector<const char*> instanceExtensionNames = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME // This will expoese the vulkan debug APIs to the application
};

std::vector<const char*> layerNames = {
    // "VK_LAYER_LUNARG_api_dump" // This layer prints API calls, parameters, and values to the identified output stream.
    // If names not correct: "VK_KHR_LUNARG_api_dump", it will report `VK_ERROR_LAYER_NOT_PRESENT`,
    // which fails at `vkCreateInstance()`.

    "VK_LAYER_GOOGLE_threading", /* Ensuring the simultaneous use of objects using calls under multiple threads.It reports
                                  * threading rule violations and enforces a mutex for such calls, allowing an application
                                  * to continue running without crashing.
                                  */
    "VK_LAYER_LUNARG_api_dump",
    "VK_LAYER_LUNARG_core_validation", /* Validating descriptor set, pipeline state, dynamic state, GPU memeory, object
                                        * binding, command buffers, graphics/compute pipeline.
                                        */
    "VK_LAYER_LUNARG_image", // Validating texture formats, rendering target formats.
    "VK_LAYER_LUNARG_object_tracker", // Tracking object creation/destruction/reference, avoiding memory leak.
    "VK_LAYER_LUNARG_parameter_validation", // Ensuring all the params passed to the API are correct and up to expectation.
    "VK_LAYER_LUNARG_swapchain", /* Validating WSI swapchain extension.Eg.check whether the WSI extension is available
                                  *  before its functions could be used.
                                  */
    "VK_LAYER_GOOGLE_unique_objects" /* Driver may return the same handle for multiple objects that it consider equivalent,
                                      * This layer packs the Vulkan objects into a unique identifier at the time of
                                      * creation and unpacks them when application uses it. Must be last in the chain
                                      * of validation layer, making it closer to the display driver.
                                      */
};

std::vector<const char*> deviceExtensionNames = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

int main(int argc, char** argv)
{
    VulkanApplication* appObj = VulkanApplication::GetInstance();
    appObj->initialize();
    appObj->prepare();
    appObj->render();
    appObj->deInitialize();

    // std::cout << "Hello CMake." << std::endl;
    // return 0;
}
