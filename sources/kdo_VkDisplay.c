/* *********************************************************************************** */
/*       :::    ::: ::::::::  :::::::::   ::::::::  :::    :::     :::     ::::::::::: */
/*      :+:   :+: :+:    :+: :+:    :+: :+:    :+: :+:   :+:    :+: :+:       :+:      */
/*     +:+  +:+  +:+    +:+ +:+    +:+ +:+    +:+ +:+  +:+    +:+   +:+      +:+       */
/*    +#++:++   +#+    +:+ +#+    +:+ +#+    +:+ +#++:++    +#++:++#++:     +#+        */
/*   +#+  +#+  +#+    +#+ +#+    +#+ +#+    +#+ +#+  +#+   +#+     +#+     +#+         */
/*  #+#   #+# #+#    #+# #+#    #+# #+#    #+# #+#   #+#  #+#     #+#     #+#          */
/* ###    ### ########  #########   ########  ###    ### ###     ### ###########       */
/*                        <kodokai.featheur@gmail.com>                                 */
/* *********************************************************************************** */

#include "kdo_VkDisplay.h"

static void	kdo_updateRenderCommand(Kdo_Vulkan *vk)
{
	VkCommandBufferBeginInfo		bufferBeginInfo;
	VkViewport						viewport;
	VkRect2D						scissor;
	VkRenderPassBeginInfo			renderPassInfo;
	VkClearValue					clearColor[2]	= {};

	clearColor[0].color.float32[0]		= 0.1f;
	clearColor[0].color.float32[1]		= 0.1f;
	clearColor[0].color.float32[2]		= 0.1f;
	clearColor[0].color.float32[3]		= 1.0f;
	clearColor[1].depthStencil.depth	= 1.0f;

	vkResetCommandPool(vk->device.path, vk->display.renderPool[vk->display.currentImage].path, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

	bufferBeginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	bufferBeginInfo.pNext				= NULL;
	bufferBeginInfo.flags				= 0;
	bufferBeginInfo.pInheritanceInfo	= NULL;

	viewport.x			= 0.0;
	viewport.y			= 0.0;
	viewport.width		= (float) vk->swapChain.imagesExtent.width;
	viewport.height		= (float) vk->swapChain.imagesExtent.height;
	viewport.minDepth	= 0.0;
	viewport.maxDepth	= 1.0;

	scissor.offset.x	= 0;
	scissor.offset.y	= 0;
	scissor.extent		= vk->swapChain.imagesExtent;

	if (vkBeginCommandBuffer(vk->display.renderPool[vk->display.currentImage].main, &bufferBeginInfo) != VK_SUCCESS)
		kdo_cleanup(vk, "Start recording command buffer failed", 30);

	renderPassInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext					= NULL;
	renderPassInfo.renderPass				= vk->renderPass.path;
	renderPassInfo.framebuffer				= vk->framebuffer.path[vk->display.currentImage];
	renderPassInfo.renderArea.offset.x		= 0;
	renderPassInfo.renderArea.offset.y		= 0;
	renderPassInfo.renderArea.extent		= vk->swapChain.imagesExtent;
	renderPassInfo.clearValueCount			= 2;
	renderPassInfo.pClearValues				= clearColor;
	vkCmdBeginRenderPass(vk->display.renderPool[vk->display.currentImage].main, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdSetViewport(vk->display.renderPool[vk->display.currentImage].main, 0, 1, &viewport);
	vkCmdSetScissor(vk->display.renderPool[vk->display.currentImage].main, 0, 1, &scissor);
	vkCmdBindPipeline(vk->display.renderPool[vk->display.currentImage].main, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->graphicsPipeline.path);

	vkCmdPushConstants(vk->display.renderPool[vk->display.currentImage].main, vk->graphicsPipeline.layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Kdo_VkPush), &vk->compute.push);

	vkCmdDraw(vk->display.renderPool[vk->display.currentImage].main, vk->compute.push.cubeSize * vk->compute.push.cubeSize, vk->compute.push.cubeSize, 0, 0);

	vkCmdEndRenderPass(vk->display.renderPool[vk->display.currentImage].main);

	if (vkEndCommandBuffer(vk->display.renderPool[vk->display.currentImage].main) != VK_SUCCESS)
		kdo_cleanup(vk, "Recording command buffer failed", 31);
}

static void	kdo_drawFrame(Kdo_Vulkan *vk)
{
	VkResult				result;
    VkSubmitInfo            submitInfo;
    VkPresentInfoKHR        presentInfo;
	VkSemaphore				imageAvailable[] = {vk->semaphore.imageAvailable[vk->display.currentFrame]};
	VkSemaphore				renderFinished[] = {vk->semaphore.renderFinished[vk->display.currentFrame]};
	VkPipelineStageFlags	waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	vkWaitForFences(vk->device.path, 1, vk->fence.renderFinished + vk->display.currentFrame, VK_TRUE, UINT64_MAX);

	result = vkAcquireNextImageKHR(vk->device.path, vk->swapChain.path, UINT64_MAX, vk->semaphore.imageAvailable[vk->display.currentFrame], VK_NULL_HANDLE, &vk->display.currentImage);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		kdo_recreateSwapchain(vk);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		kdo_cleanup(vk, "Acquire image failed\n", 32);

	//syncronisation
	if (vk->display.imageToFrame[vk->display.currentImage])
	{
		vkWaitForFences(vk->device.path, 1, vk->display.imageToFrame[vk->display.currentImage]->renderFinishedFence, VK_TRUE, UINT64_MAX);
		vk->display.imageToFrame[vk->display.currentImage]->path = NULL;
	}
	if (vk->display.frameToImage[vk->display.currentFrame].path)
		*vk->display.frameToImage[vk->display.currentFrame].path = NULL;
	vk->display.imageToFrame[vk->display.currentImage] = vk->display.frameToImage + vk->display.currentFrame;
	vk->display.frameToImage[vk->display.currentFrame].path = vk->display.imageToFrame + vk->display.currentImage;

	kdo_compute(vk);
	kdo_updateRenderCommand(vk);

	submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext					= NULL;
	submitInfo.waitSemaphoreCount		= 1;
	submitInfo.pWaitSemaphores			= imageAvailable;
	submitInfo.pWaitDstStageMask		= waitStages;
	submitInfo.commandBufferCount		= 1;
	submitInfo.pCommandBuffers			= &vk->display.renderPool[vk->display.currentImage].main;
	submitInfo.signalSemaphoreCount		= 1;
	submitInfo.pSignalSemaphores		= renderFinished;
	vkResetFences(vk->device.path, 1, vk->fence.renderFinished + vk->display.currentFrame);
	if (vkQueueSubmit(vk->device.queues[GRAPHIC_QUEUE].path, 1, &submitInfo, vk->fence.renderFinished[vk->display.currentFrame]) != VK_SUCCESS)
		kdo_cleanup(vk, "Queue submit failed", 33);

	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext				= NULL;
	presentInfo.waitSemaphoreCount	= 1;
	presentInfo.pWaitSemaphores		= renderFinished;
	presentInfo.swapchainCount		= 1;
	presentInfo.pSwapchains			= &vk->swapChain.path;
	presentInfo.pImageIndices		= &vk->display.currentImage;
	presentInfo.pResults				= NULL;
	result = vkQueuePresentKHR(vk->device.queues[PRESENT_QUEUE].path, &presentInfo);

	if (vk->display.windowResized || result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		kdo_recreateSwapchain(vk);
	else if (result != VK_SUCCESS)
		kdo_cleanup(vk, "Present image failed", 34);

	vk->display.currentFrame = (vk->display.currentFrame + 1) % vk->display.maxParallelFrame;
}

void	kdo_initRenderPool(Kdo_Vulkan *vk)
{
	VkCommandPoolCreateInfo			commandPoolInfo;
	VkCommandBufferAllocateInfo		bufferAllocInfo;

	KDO_VK_ALLOC(vk->display.renderPool, malloc(vk->swapChain.imagesCount * sizeof(Kdo_VkRenderPool)));

	commandPoolInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext				= NULL;
	commandPoolInfo.flags				= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	commandPoolInfo.queueFamilyIndex	= vk->device.queues[GRAPHIC_QUEUE].familyIndex;

	bufferAllocInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocInfo.pNext				= NULL;
	bufferAllocInfo.commandBufferCount	= 1;

	for (uint32_t i = 0; i < vk->swapChain.imagesCount; i++)
	{
		if (vkCreateCommandPool(vk->device.path, &commandPoolInfo, NULL, &vk->display.renderPool[i].path) != VK_SUCCESS)
			kdo_cleanup(vk, "Render pool creation failed", 28);

		bufferAllocInfo.commandPool	= vk->display.renderPool[i].path;
		bufferAllocInfo.level		= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		if (vkAllocateCommandBuffers(vk->device.path, &bufferAllocInfo, &vk->display.renderPool[i].main) != VK_SUCCESS)
			kdo_cleanup(vk, "Render command Buffers allocation failed", 29);
	}
}

void	kdo_mainLoop(Kdo_Vulkan *vk)
{
	kdo_initRenderPool(vk);

	while (!glfwWindowShouldClose(vk->window.path))
	{
		glfwPollEvents();
		kdo_drawFrame(vk);
	}
}
