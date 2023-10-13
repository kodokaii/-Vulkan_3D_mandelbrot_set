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

#include "kdo_VkMemory.h"

void	kdo_vkNewBuffer(Kdo_Vulkan *vk, VkMemoryPropertyFlags memoryFlags, VkBufferUsageFlags usage, VkDeviceSize size, Kdo_VkBuffer *buffer)
{
	VkBufferCreateInfo	bufferInfo;

	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext					= NULL;
	bufferInfo.flags					= 0;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount	= 0;
	bufferInfo.pQueueFamilyIndices		= NULL;
	if (kdo_newBuffer(vk->device.path, vk->physicalDevice.memoryProperties, memoryFlags, WAIT_DEVICE, &bufferInfo, buffer))
		kdo_cleanup(vk, "Buffer creation failed", 23);
}

void	kdo_vkNewSetBuffer(Kdo_Vulkan *vk, VkMemoryPropertyFlags memoryFlags, VkBufferUsageFlags usage, VkDeviceSize elementSize, VkDeviceSize size, Kdo_VkSetBuffer *buffer)
{
	VkBufferCreateInfo	bufferInfo;

	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext					= NULL;
	bufferInfo.flags					= 0;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount	= 0;
	bufferInfo.pQueueFamilyIndices		= NULL;
	if (kdo_newSetBuffer(vk->device.path, vk->physicalDevice.memoryProperties, memoryFlags, WAIT_DEVICE, elementSize, &bufferInfo, buffer))
		kdo_cleanup(vk, "Set Buffer creation failed", 24);
}

void	kdo_vkNewSetImageBuffer(Kdo_Vulkan *vk, VkMemoryPropertyFlags memoryFlags, uint32_t maxImageCount, VkDeviceSize size, Kdo_VkSetImageBuffer *imageBuffer)
{
	VkImageCreateInfo	imageInfo;

	imageInfo.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext					= NULL;
	imageInfo.flags					= 0;
	imageInfo.imageType				= VK_IMAGE_TYPE_2D;
	imageInfo.format				= VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.extent.width			= vk->physicalDevice.properties.limits.maxImageDimension2D;
	imageInfo.extent.height			= vk->physicalDevice.properties.limits.maxImageDimension2D;
	imageInfo.extent.depth			= 1;
	imageInfo.mipLevels				= 1;
	imageInfo.arrayLayers			= 1;
	imageInfo.samples				= VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling				= VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage					= VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount	= 0;
	imageInfo.pQueueFamilyIndices	= NULL;
	imageInfo.initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
	
	if (kdo_newSetImageBuffer(vk->device.path, vk->physicalDevice.memoryProperties, memoryFlags, WAIT_DEVICE, size, maxImageCount, &imageInfo, imageBuffer))
		kdo_cleanup(vk, "Set Image Buffer creation failed", 25);
}


void	kdo_vkFreeBuffer(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer)
{
	kdo_freeBuffer(vk->device.path, buffer);
}

void	kdo_vkFreeSetBuffer(Kdo_Vulkan *vk, Kdo_VkSetBuffer *buffer)
{
	kdo_freeSetBuffer(vk->device.path, buffer);
}

void	kdo_vkFreeSetImageBuffer(Kdo_Vulkan *vk, Kdo_VkSetImageBuffer *imageBuffer)
{
	kdo_freeSetImageBuffer(vk->device.path, imageBuffer);
}


void	kdo_vkWriteBufferData(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize)
{
	if (kdo_writeBufferData(buffer, offset, data, dataSize))
		kdo_cleanup(vk, "Write buffer failed", 26);
}

void	kdo_vkPushBufferData(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer, void *data, VkDeviceSize dataSize)
{
	if (kdo_pushBufferData(buffer, data, dataSize))
		kdo_cleanup(vk, "Push buffer failed", 27);
}

void	kdo_vkPushSetBufferData(Kdo_Vulkan *vk, Kdo_VkSetBuffer *buffer, void *data, uint32_t *index)
{
	if (kdo_pushSetBufferData(buffer, data, index))
		kdo_cleanup(vk, "Push set buffer failed", 28);
}

void	kdo_vkPushSetImageBufferPath(Kdo_Vulkan *vk, Kdo_VkSetImageBuffer *imageBuffer, char *path, uint32_t *index)
{
	if (kdo_pushSetImageBufferPath(imageBuffer, path, index))
		kdo_cleanup(vk, "Push set image buffer failed", 29);
}


void	kdo_vkBeginUniqueCommand(Kdo_Vulkan *vk, VkCommandBuffer *commandBuffer)
{
	if (kdo_beginUniqueCommand(vk->device.path, vk->core.transferPool, commandBuffer))
		kdo_cleanup(vk, "Begin unique command failed", 30);
}

uint32_t	kdo_vkFindMemoryType(Kdo_Vulkan *vk, uint32_t typeFilter, VkMemoryPropertyFlags memoryFlags)
{
	uint32_t	memoryType;

	if (kdo_findMemoryType(typeFilter, vk->physicalDevice.memoryProperties, memoryFlags, &memoryType))
		kdo_cleanup(vk, "Couldn't find memory type", 31);

	return (memoryType);
}

void	kdo_vkEndUniqueCommand(Kdo_Vulkan *vk, VkCommandBuffer *commandBuffer)
{
	if (kdo_endUniqueCommand(commandBuffer, vk->device.path, vk->core.transferPool, vk->device.queues[TRANSFER_QUEUE].path))
		kdo_cleanup(vk, "End unique command failed", 32);
}

void	kdo_vkUpdateBuffer(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer, VkDeviceSize offset, VkDeviceSize size)
{
	if (kdo_updateBuffer(vk->device.path, vk->physicalDevice.memoryProperties, vk->core.transferPool, vk->device.queues[TRANSFER_QUEUE].path, buffer, offset, size))
		kdo_cleanup(vk, "Update buffer failed", 33);
}

void	kdo_vkUpdateAllBuffer(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer)
{
	if (kdo_updateAllBuffer(vk->device.path, vk->physicalDevice.memoryProperties, vk->core.transferPool, vk->device.queues[TRANSFER_QUEUE].path, buffer))
		kdo_cleanup(vk, "Update all buffer failed", 34);
}

void	kdo_vkUpdateSetBuffer(Kdo_Vulkan *vk, Kdo_VkSetBuffer *buffer)
{
	if (kdo_updateSetBuffer(vk->device.path, vk->physicalDevice.memoryProperties, vk->core.transferPool, vk->device.queues[TRANSFER_QUEUE].path, buffer))
		kdo_cleanup(vk, "Update set buffer failed", 35);
}

void	kdo_vkUpdateSetImageBuffer(Kdo_Vulkan *vk, Kdo_VkSetImageBuffer *imageBuffer, VkImageLayout layout)
{
	if (kdo_updateSetImageBuffer(vk->device.path, vk->physicalDevice.memoryProperties, vk->core.transferPool, vk->device.queues[TRANSFER_QUEUE].path, imageBuffer, layout))
		kdo_cleanup(vk, "Update set image buffer failed", 36);
}
