#pragma once

#include "Headers.h"

/***************COMMAND BUFFER WRAPPERS***************/
class CommandBufferMgr
{
public:
    static void allocCommandBuffer(const VkDevice* device, const VkCommandPool cmdPool,VkCommandBuffer* cmdBuffer, const VkCommandBufferAllocateInfo* cmdBufferAllocateInfo = NULL);
    static void beginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferBeginInfo* inCmdBufferBeginInfo = NULL);
    static void endCommandBuffer(VkCommandBuffer cmdBuffer);
    static void submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* cmdBufferList, const VkSubmitInfo* submitInfo = NULL, const VkFence& fence = VK_NULL_HANDLE);
};