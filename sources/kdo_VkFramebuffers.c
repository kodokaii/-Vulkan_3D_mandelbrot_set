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

#include "kdo_VkFramebuffers.h"

static void	kdo_initDepthBuffer(Kdo_Vulkan *vk)
{
	VkImageCreateInfo		imageInfo;
	VkImageViewCreateInfo   viewInfo;
	VkMemoryRequirements	memRequirements;
	VkMemoryAllocateInfo	allocInfo;

	imageInfo.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext					= NULL;
	imageInfo.flags					= 0;
	imageInfo.imageType				= VK_IMAGE_TYPE_2D;
	imageInfo.format				= vk->framebuffer.depth.format;
	imageInfo.extent.width			= vk->swapChain.imagesExtent.width;
	imageInfo.extent.height			= vk->swapChain.imagesExtent.height;
	imageInfo.extent.depth			= 1;
	imageInfo.mipLevels				= 1;
	imageInfo.arrayLayers			= 1;
	imageInfo.samples				= VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling				= VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount	= 0;
	imageInfo.pQueueFamilyIndices	= NULL;
	imageInfo.initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
	if (vkCreateImage(vk->device.path, &imageInfo, NULL, &vk->framebuffer.depth.image) != VK_SUCCESS)
		kdo_cleanup(vk, "Depth image creation failed", 17);

	vkGetImageMemoryRequirements(vk->device.path, vk->framebuffer.depth.image, &memRequirements);
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext				= NULL;
	allocInfo.allocationSize	= memRequirements.size;
	allocInfo.memoryTypeIndex	= kdo_vkFindMemoryType(vk, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(vk->device.path, &allocInfo, NULL, &vk->framebuffer.depth.memory) != VK_SUCCESS)
		kdo_cleanup(vk, "Depth memory allocation failed", 18);

	vkBindImageMemory(vk->device.path, vk->framebuffer.depth.image, vk->framebuffer.depth.memory, 0);

	viewInfo.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext                              = NULL;
	viewInfo.flags                              = 0;
	viewInfo.image                              = vk->framebuffer.depth.image;
	viewInfo.viewType                           = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format                             = vk->framebuffer.depth.format;
	viewInfo.components.r                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask        = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo.subresourceRange.baseMipLevel      = 0;
	viewInfo.subresourceRange.levelCount        = 1;
	viewInfo.subresourceRange.baseArrayLayer    = 0;
	viewInfo.subresourceRange.layerCount        = 1;
	if (vkCreateImageView(vk->device.path, &viewInfo, NULL, &vk->framebuffer.depth.view) != VK_SUCCESS)
		kdo_cleanup(vk, "Depth view creation failed", 19);
}

void    kdo_initFramebuffers(Kdo_Vulkan *vk)
{
	VkFramebufferCreateInfo	framebufferInfo;

	kdo_initDepthBuffer(vk);

	KDO_VK_ALLOC(vk->framebuffer.path, malloc(vk->swapChain.imagesCount * sizeof(VkFramebuffer)));

	for (uint32_t i = 0; i < vk->swapChain.imagesCount; i++)
	{
		VkImageView	attachments[2]	= {vk->swapChain.views[i], vk->framebuffer.depth.view};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = NULL;
		framebufferInfo.flags = 0;
		framebufferInfo.renderPass = vk->renderPass.path;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = vk->swapChain.imagesExtent.width;
		framebufferInfo.height = vk->swapChain.imagesExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(vk->device.path, &framebufferInfo, NULL, vk->framebuffer.path + i) != VK_SUCCESS)
			kdo_cleanup(vk, "Framebuffer creation failed", 20);
	}
}
