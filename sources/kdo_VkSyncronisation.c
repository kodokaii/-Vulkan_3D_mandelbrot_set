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

#include "kdo_VkSyncronisation.h"

void	kdo_initSyncronisation(Kdo_Vulkan *vk)
{
	VkSemaphoreCreateInfo	semaphoreInfo;
	VkFenceCreateInfo		fenceInfo;

	if ((vk->display.maxParallelFrame = vk->swapChain.imagesCount - 1) < 1)
		vk->display.maxParallelFrame = 1;

	KDO_VK_ALLOC(vk->semaphore.imageAvailable, malloc(vk->display.maxParallelFrame * sizeof(VkSemaphore)));

	KDO_VK_ALLOC(vk->semaphore.renderFinished, malloc(vk->display.maxParallelFrame * sizeof(VkSemaphore)));

	KDO_VK_ALLOC(vk->fence.renderFinished, malloc(vk->display.maxParallelFrame * sizeof(VkFence)));

	KDO_VK_ALLOC(vk->display.frameToImage, calloc(vk->display.maxParallelFrame, sizeof(Kdo_SynImage)));

	KDO_VK_ALLOC(vk->display.imageToFrame, calloc(vk->swapChain.imagesCount, sizeof(Kdo_SynImage *)));


	for (uint32_t i = 0; i < vk->display.maxParallelFrame; i++)
	{
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext	= NULL;
		semaphoreInfo.flags	= 0;
		if (vkCreateSemaphore(vk->device.path, &semaphoreInfo, NULL, vk->semaphore.imageAvailable + i) != VK_SUCCESS)
			kdo_cleanup(vk, "Semaphore creation failed", 21);
		if (vkCreateSemaphore(vk->device.path, &semaphoreInfo, NULL, vk->semaphore.renderFinished + i) != VK_SUCCESS)
			kdo_cleanup(vk, "Semaphore creation failed", 21);

		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext	= NULL;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (vkCreateFence(vk->device.path, &fenceInfo, NULL, vk->fence.renderFinished + i) != VK_SUCCESS)
			kdo_cleanup(vk, "Fence creation failed", 22);

		vk->display.frameToImage[i].renderFinishedFence	= vk->fence.renderFinished + i;
	}
}
