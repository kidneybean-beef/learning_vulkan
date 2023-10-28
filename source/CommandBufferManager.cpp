#include "CommandBufferManager.h"

void CommandBufferMgr::allocCommandBuffer(const VkDevice* device, const VkCommandPool cmdPool, VkCommandBuffer* cmdBuffer, const VkCommandBufferAllocateInfo* inCmdBufferAllocateInfo)
{
    VkResult result;

    // If command information is available, then use it as it is.
    if (inCmdBufferAllocateInfo) {
        result = vkAllocateCommandBuffers(*device, inCmdBufferAllocateInfo, cmdBuffer);
        assert(!result);
        return;
    }

    // Default implementation, create the command buffer
    // allocation info and use the supplied params into it.
    VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {};
    cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocateInfo.pNext = NULL;
    cmdBufferAllocateInfo.commandPool = cmdPool;
    cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocateInfo.commandBufferCount = (uint32_t)sizeof(cmdBuffer) / sizeof(VkCommandBuffer);

    result = vkAllocateCommandBuffers(*device, &cmdBufferAllocateInfo, cmdBuffer);
    assert(!result);
}

void CommandBufferMgr::beginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferBeginInfo* inCmdBufferBeginInfo) {
    VkResult result;
    
    // If the user has specified the custom command buffer then just use it.
    if (inCmdBufferBeginInfo)
    {
        result = vkBeginCommandBuffer(cmdBuffer, inCmdBufferBeginInfo);
        assert(result == VK_SUCCESS);
        return;
    }

    // Otherwise, use the default implementation.
    VkCommandBufferInheritanceInfo cmdBufferInheritanceInfo = {};
    cmdBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    cmdBufferInheritanceInfo.pNext = NULL;
    cmdBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
    cmdBufferInheritanceInfo.subpass = 0;
    cmdBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
    cmdBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
    cmdBufferInheritanceInfo.queryFlags = 0;
    cmdBufferInheritanceInfo.pipelineStatistics = 0;

    VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.pNext = NULL;
    cmdBufferBeginInfo.flags = 0;
    cmdBufferBeginInfo.pInheritanceInfo = &cmdBufferInheritanceInfo;
    
    result = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    assert(result == VK_SUCCESS);
}

void CommandBufferMgr::endCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkResult result;
    result = vkEndCommandBuffer(commandBuffer);
    assert(result == VK_SUCCESS);
}

void CommandBufferMgr::submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* commandBuffer, const VkSubmitInfo* inSubmitInfo, const VkFence& fence)
{
    VkResult result;

    // If submit information is available, use it as it is.
    // This assumes that the commands are already specified in the structure,
    // hence ignore command buffer.

    if (inSubmitInfo) {
        result = vkQueueSubmit(queue, 1, inSubmitInfo, fence);
        assert(!result);

        result = vkQueueWaitIdle(queue);
        assert(!result);
        return;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.commandBufferCount = (uint32_t)sizeof(commandBuffer) / sizeof(VkCommandBuffer);
    submitInfo.pCommandBuffers = commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;

    result = vkQueueSubmit(queue, 1, &submitInfo, fence);
    assert(!result);
    result = vkQueueWaitIdle(queue);
    assert(!result);
}