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

#include "kdo_VkBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

VkResult		kdo_newGPUMemory(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, uint32_t memoryFilter, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize size, Kdo_VkGPUMemory *memory)
{
	VkMemoryAllocateInfo	allocInfo; 
	VkResult				returnCode;

	memory->flags		= memoryFlags;	
	memory->waitFlags	= waitFlags;
	memory->size		= size;

	allocInfo.sType             = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext             = NULL;
	allocInfo.allocationSize    = size;
	KDO_VK_ASSERT(kdo_findMemoryType(memoryFilter, memoryProperties, memoryFlags, &allocInfo.memoryTypeIndex));
	KDO_VK_ASSERT(vkAllocateMemory(device, &allocInfo, NULL, &memory->path));

	return (VK_SUCCESS);
}

VkResult		kdo_newGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkBufferCreateInfo *bufferInfo, Kdo_VkGPUBuffer *buffer)
{
	VkMemoryRequirements	memoryRequirements;
	VkResult				returnCode;

	buffer->info		= *bufferInfo;
	buffer->fillSize	= 0;

	bufferInfo->usage	|= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	KDO_VK_ASSERT(vkCreateBuffer(device, bufferInfo, NULL, &buffer->path) != VK_SUCCESS);
	vkGetBufferMemoryRequirements(device, buffer->path, &memoryRequirements);

	KDO_VK_ASSERT(kdo_newGPUMemory(device, memoryProperties, memoryRequirements.memoryTypeBits, memoryFlags, waitFlags, memoryRequirements.size, &buffer->memory));
	KDO_VK_ASSERT(vkBindBufferMemory(device, buffer->path, buffer->memory.path, 0));

	return (VK_SUCCESS);
}

VkResult		kdo_newGPUStagingBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, Kdo_VkGPUBuffer *stagingBuffer, void *data, VkDeviceSize dataSize)
{
	VkBufferCreateInfo	bufferInfo;
	VkResult			returnCode;

	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext					= NULL;
	bufferInfo.flags					= 0;
	bufferInfo.size						= dataSize;
	bufferInfo.usage					= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount	= 0;
	bufferInfo.pQueueFamilyIndices		= NULL;
	KDO_VK_ASSERT(kdo_newGPUBuffer(device, memoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, WAIT_NONE, &bufferInfo, stagingBuffer));
	KDO_VK_ASSERT(kdo_writeHostGPUMemory(device, &stagingBuffer->memory, 0, data, dataSize));
	stagingBuffer->fillSize	= dataSize;
	
	return (VK_SUCCESS);
}

VkResult		kdo_newGPUImage(VkDevice device, VkImageLayout layout, VkImageCreateInfo *imageInfo, Kdo_VkGPUImage *image)
{
	VkMemoryRequirements    memRequirements;
	VkResult				returnCode;

	imageInfo->usage	|= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	KDO_VK_ASSERT(vkCreateImage(device, imageInfo, NULL, &image->path));
	vkGetImageMemoryRequirements(device, image->path, &memRequirements);
	
	image->view			= VK_NULL_HANDLE;
	image->extent		= imageInfo->extent;
	image->layout		= layout;
	image->alignment	= memRequirements.alignment;
	image->size			= memRequirements.size;

	return (VK_SUCCESS);
}

VkResult		kdo_newGPUImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize size, uint32_t maxImageCount, VkImageCreateInfo  *imageInfo, Kdo_VkGPUImageBuffer *imageBuffer)
{
	VkResult	returnCode;
	uint32_t	memoryFilter;

	if (!(imageBuffer->imageArray	= calloc(maxImageCount, sizeof(Kdo_VkGPUImage))))
		return (VK_ERROR_OUT_OF_HOST_MEMORY);
	imageBuffer->info			= *imageInfo;
	imageBuffer->fillSize		= 0;
	imageBuffer->maxImageCount	= maxImageCount;
	imageBuffer->imageCount		= 0;

	kdo_findMemoryFilterImage(device, imageInfo, &memoryFilter);
	KDO_VK_ASSERT(kdo_newGPUMemory(device, memoryProperties, memoryFilter, memoryFlags, waitFlags, size, &imageBuffer->memory));

	return (VK_SUCCESS);
}

VkResult		kdo_newCPUBuffer(VkDeviceSize size, Kdo_VkCPUBuffer *buffer)
{
	if (!(buffer->path	= malloc(size)))
		return (VK_ERROR_OUT_OF_HOST_MEMORY);
	buffer->size		= size;
	buffer->fillSize	= 0;

	return (VK_SUCCESS);
}

VkResult		kdo_newCPUImageBuffer(uint32_t	maxImageCount, Kdo_VkCPUImageBuffer *imageBuffer)
{
	if (!(imageBuffer->path	= calloc(maxImageCount, sizeof(char *))))
		return (VK_ERROR_OUT_OF_HOST_MEMORY);
	imageBuffer->maxImageCount	= maxImageCount;
	imageBuffer->imageCount		= 0;

	return (VK_SUCCESS);
}

VkResult	kdo_newBSTNode(uint32_t index, Kdo_VkBSTNode **node)
{
	if (!(*node = malloc(sizeof(Kdo_VkBSTNode))))
		return (VK_ERROR_OUT_OF_HOST_MEMORY);
	(*node)->left	= NULL;
	(*node)->right	= NULL;
	(*node)->index	= index;

	return (VK_SUCCESS);
}

VkResult	kdo_newBST(VkDeviceSize elementSize, Kdo_VkCPUBuffer *CPUBuffer, Kdo_VkBST *bst)
{
	bst->root			= NULL;
	bst->CPUBuffer		= CPUBuffer;
	bst->elementSize	= elementSize;

	return (VK_SUCCESS);
}

VkResult	kdo_newImageBST(Kdo_VkCPUImageBuffer *CPUImageBuffer, Kdo_VkImageBST *imageBst)
{
	imageBst->root				= NULL;
	imageBst->CPUImageBuffer	= CPUImageBuffer;

	return (VK_SUCCESS);
}

VkResult	kdo_newBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkBufferCreateInfo *bufferInfo, Kdo_VkBuffer *buffer)
{
	VkResult	returnCode;

	KDO_VK_ASSERT(kdo_newGPUBuffer(device, memoryProperties, memoryFlags, waitFlags, bufferInfo, &buffer->GPU));
	KDO_VK_ASSERT(kdo_newCPUBuffer(bufferInfo->size, &buffer->CPU));

	return (VK_SUCCESS);
}

VkResult	kdo_newSetBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize elementSize, VkBufferCreateInfo *bufferInfo, Kdo_VkSetBuffer *buffer)
{
	VkResult	returnCode;

	KDO_VK_ASSERT(kdo_newGPUBuffer(device, memoryProperties, memoryFlags, waitFlags, bufferInfo, &buffer->GPU));
	KDO_VK_ASSERT(kdo_newCPUBuffer(bufferInfo->size, &buffer->CPU));
	KDO_VK_ASSERT(kdo_newBST(elementSize, &buffer->CPU, &buffer->BST));

	return (VK_SUCCESS);
}

VkResult	kdo_newSetImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize size, uint32_t maxImageCount, VkImageCreateInfo  *imageInfo, Kdo_VkSetImageBuffer *imageBuffer)
{
	VkResult	returnCode;

	KDO_VK_ASSERT(kdo_newGPUImageBuffer(device, memoryProperties, memoryFlags, waitFlags, size, maxImageCount, imageInfo, &imageBuffer->GPU));
	KDO_VK_ASSERT(kdo_newCPUImageBuffer(maxImageCount, &imageBuffer->CPU));
	KDO_VK_ASSERT(kdo_newImageBST(&imageBuffer->CPU, &imageBuffer->BST));

	return (VK_SUCCESS);
}


void	kdo_freeGPUMemory(VkDevice device, Kdo_VkGPUMemory *memory)
{
	if (memory->waitFlags & WAIT_DEVICE)
		vkDeviceWaitIdle(device);
	KDO_DESTROY(vkFreeMemory, device, memory->path);
	memory->size	= 0;
}

void	kdo_freeGPUBuffer(VkDevice device, Kdo_VkGPUBuffer *buffer)
{
	KDO_DESTROY(vkDestroyBuffer, device, buffer->path);
	kdo_freeGPUMemory(device, &buffer->memory);
	buffer->fillSize	= 0;
}

void	kdo_freeGPUImage(VkDevice device, Kdo_VkGPUImage *image)
{
	KDO_DESTROY(vkDestroyImageView, device, image->view);
	KDO_DESTROY(vkDestroyImage, device, image->path);
	image->size	= 0;
}

void	kdo_freeGPUImageBuffer(VkDevice device, Kdo_VkGPUImageBuffer *imageBuffer)
{
	for (uint32_t i = 0; i < imageBuffer->imageCount; i++)
		kdo_freeGPUImage(device, &imageBuffer->imageArray[i]);
	KDO_FREE(imageBuffer->imageArray);
	kdo_freeGPUMemory(device, &imageBuffer->memory);
	imageBuffer->fillSize		= 0;
	imageBuffer->maxImageCount	= 0;
	imageBuffer->imageCount		= 0;
}

void	kdo_freeCPUBuffer(Kdo_VkCPUBuffer *buffer)
{
	KDO_FREE(buffer->path);
	buffer->size		= 0;
	buffer->fillSize	= 0;
}

void	kdo_freeCPUImageBuffer(Kdo_VkCPUImageBuffer *imageBuffer)
{
	for (uint32_t i = 0; i < imageBuffer->imageCount; i++)
		KDO_FREE(imageBuffer->path[i]);
	KDO_FREE(imageBuffer->path);
	imageBuffer->maxImageCount	= 0;
	imageBuffer->imageCount		= 0;
}

void	kdo_freeBSTNode(Kdo_VkBSTNode **node)
{
	if ((*node)->left)
		kdo_freeBSTNode(&((*node)->left));
	if ((*node)->right)
		kdo_freeBSTNode(&((*node)->right));
	KDO_FREE(*node);
}

void	kdo_freeBST(Kdo_VkBST *bst)
{
	kdo_freeBSTNode(&bst->root);
}

void	kdo_freeImageBST(Kdo_VkImageBST *imageBst)
{
	kdo_freeBSTNode(&imageBst->root);
}

void	kdo_freeBuffer(VkDevice device, Kdo_VkBuffer *buffer)
{
	kdo_freeGPUBuffer(device, &buffer->GPU);
	kdo_freeCPUBuffer(&buffer->CPU);
}

void	kdo_freeSetBuffer(VkDevice device, Kdo_VkSetBuffer *buffer)
{
	kdo_freeGPUBuffer(device, &buffer->GPU);
	kdo_freeCPUBuffer(&buffer->CPU);
	kdo_freeBST(&buffer->BST);
}

void	kdo_freeSetImageBuffer(VkDevice device, Kdo_VkSetImageBuffer *imageBuffer)
{
	kdo_freeGPUImageBuffer(device, &imageBuffer->GPU);
	kdo_freeCPUImageBuffer(&imageBuffer->CPU);
	kdo_freeImageBST(&imageBuffer->BST);
}


VkResult	kdo_beginUniqueCommand(VkDevice device, VkCommandPool commandPool, VkCommandBuffer *commandBuffer)
{
	VkCommandBufferAllocateInfo	allocInfo;
	VkCommandBufferBeginInfo	beginInfo;
	VkResult					returnCode;

	allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext					= NULL;
	allocInfo.commandPool			= commandPool;
	allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount	= 1;
	KDO_VK_ASSERT(vkAllocateCommandBuffers(device, &allocInfo, commandBuffer));

	beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext				= NULL;
	beginInfo.flags				= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo	= NULL;
	KDO_VK_ASSERT(vkBeginCommandBuffer(*commandBuffer, &beginInfo));

	return (VK_SUCCESS);
}

VkResult	kdo_findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, uint32_t *memoryType)
{
	*memoryType = 0;
	while ( *memoryType < memoryProperties.memoryTypeCount)
	{
		if ((typeFilter & (1 << *memoryType)) && (memoryProperties.memoryTypes[*memoryType].propertyFlags & memoryFlags) == memoryFlags)
			return (VK_SUCCESS);

		*memoryType += 1;
	}
	return (VK_ERROR_UNKNOWN);
}

VkResult	kdo_findMemoryFilterImage(VkDevice device, VkImageCreateInfo *imageInfo, uint32_t *memoryFilter)
{
      VkImage				image;
      VkMemoryRequirements	memoryRequirements;
	  VkResult				returnCode;				

      KDO_VK_ASSERT(vkCreateImage(device, imageInfo, NULL, &image));
      vkGetImageMemoryRequirements(device, image, &memoryRequirements);
	  *memoryFilter	= memoryRequirements.memoryTypeBits;
      vkDestroyImage(device, image, NULL);

	  return (VK_SUCCESS);
}

VkDeviceSize	kdo_minSize(VkDeviceSize val1, VkDeviceSize val2)
{
	if (val1 <= val2)
		return (val1);
	return (val2);
}

VkDeviceSize	kdo_maxSize(VkDeviceSize val1, VkDeviceSize val2)
{
	if (val1 <= val2)
		return (val2);
	return (val1);
}

VkResult	kdo_cmdImageBarrier(VkCommandBuffer commandBuffer, VkImage image, \
								    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, \
									VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, \
									VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier	imageBarrierInfo;

	imageBarrierInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrierInfo.pNext                           = NULL;
	imageBarrierInfo.srcAccessMask                   = srcAccessMask;
	imageBarrierInfo.dstAccessMask                   = dstAccessMask;
	imageBarrierInfo.oldLayout                       = oldLayout;
	imageBarrierInfo.newLayout                       = newLayout;
	imageBarrierInfo.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	imageBarrierInfo.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	imageBarrierInfo.image                           = image;
	imageBarrierInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrierInfo.subresourceRange.baseMipLevel   = 0;
	imageBarrierInfo.subresourceRange.levelCount     = 1;
	imageBarrierInfo.subresourceRange.baseArrayLayer = 0;
	imageBarrierInfo.subresourceRange.layerCount     = 1;
	vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, &imageBarrierInfo);

	return (VK_SUCCESS);
}

VkResult	kdo_endUniqueCommand(VkCommandBuffer *commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue queue)
{
	VkSubmitInfo	submitInfo;
	VkResult		returnCode;

	KDO_VK_ASSERT(vkEndCommandBuffer(*commandBuffer));

	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext				= NULL;
	submitInfo.waitSemaphoreCount	= 0;
	submitInfo.pWaitSemaphores		= NULL;
	submitInfo.pWaitDstStageMask	= NULL;
	submitInfo.commandBufferCount	= 1;
	submitInfo.pCommandBuffers		= commandBuffer;
	submitInfo.signalSemaphoreCount	= 0;
	submitInfo.pSignalSemaphores	= NULL;
	KDO_VK_ASSERT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, commandBuffer);

	return (VK_SUCCESS);
}


VkResult	kdo_reallocGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *buffer, VkDeviceSize newSize)
{
	Kdo_VkGPUBuffer newBuffer;
	VkResult		returnCode;


	buffer->info.size	= kdo_maxSize(buffer->memory.size * 2, newSize);
	KDO_VK_ASSERT(kdo_newGPUBuffer(device, memoryProperties, buffer->memory.flags, buffer->memory.waitFlags, &buffer->info, &newBuffer));
	KDO_VK_ASSERT(kdo_cpyGPUBuffer(device, commandPool, queue, &newBuffer, 0, buffer, 0, kdo_minSize(buffer->fillSize, newSize)));

	kdo_freeGPUBuffer(device, buffer);
	*buffer	= newBuffer;

	return (VK_SUCCESS);
}

VkResult	kdo_writeHostGPUMemory(VkDevice device, Kdo_VkGPUMemory *memory, VkDeviceSize offset, void *data, VkDeviceSize dataSize)
{
	void            *mapMemory;
	VkResult		returnCode;

	if (dataSize == 0)
		return (VK_SUCCESS);

	if (memory->size < offset + dataSize)
		return (VK_ERROR_MEMORY_MAP_FAILED);

	KDO_VK_ASSERT(vkMapMemory(device, memory->path, offset, dataSize, 0, &mapMemory));
	memcpy(mapMemory, data, dataSize);
	vkUnmapMemory(device, memory->path);

	return (VK_SUCCESS);
}

VkResult	kdo_writeGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize)
{
	Kdo_VkGPUBuffer	stagingBuffer;
	VkResult		returnCode;
	
	if (buffer->memory.size < offset + dataSize)
		return (VK_ERROR_MEMORY_MAP_FAILED);

	if (buffer->fillSize < offset)
		return (VK_ERROR_NOT_PERMITTED_EXT);

	if (buffer->memory.flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		KDO_VK_ASSERT(kdo_writeHostGPUMemory(device, &buffer->memory, offset, data, dataSize));
		buffer->fillSize = kdo_maxSize(buffer->fillSize, offset + dataSize);
	}
	else
	{
		KDO_VK_ASSERT(kdo_newGPUStagingBuffer(device, memoryProperties, &stagingBuffer, data, dataSize));
		KDO_VK_ASSERT(kdo_cpyGPUBuffer(device, commandPool, queue, buffer, offset, &stagingBuffer, 0, dataSize));
		kdo_freeGPUBuffer(device, &stagingBuffer);
	}

	return (VK_SUCCESS);
}

VkResult	kdo_cpyGPUBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *dstBuffer, VkDeviceSize dstOffset, Kdo_VkGPUBuffer *srcBuffer, VkDeviceSize srcOffset, VkDeviceSize dataSize)
{
	VkCommandBuffer			commandBuffer;
	VkBufferCopy			copyInfo;
	VkResult				returnCode;

	if (dataSize == 0)
		return (VK_SUCCESS);

	if (srcBuffer->memory.size < srcOffset + dataSize || dstBuffer->memory.size < dstOffset + dataSize)
		return (VK_ERROR_MEMORY_MAP_FAILED);

	if (srcBuffer->fillSize < srcOffset + dataSize  || dstBuffer->fillSize < dstOffset)
		return (VK_ERROR_NOT_PERMITTED_EXT);

	KDO_VK_ASSERT(kdo_beginUniqueCommand(device, commandPool, &commandBuffer));
	copyInfo.srcOffset	= srcOffset;
	copyInfo.dstOffset	= dstOffset;
	copyInfo.size		= dataSize;
	vkCmdCopyBuffer(commandBuffer, srcBuffer->path, dstBuffer->path, 1, &copyInfo);
	KDO_VK_ASSERT(kdo_endUniqueCommand(&commandBuffer, device, commandPool, queue));

	dstBuffer->fillSize = kdo_maxSize(dstBuffer->fillSize, dstOffset + dataSize);

	return (VK_SUCCESS);
}

VkResult	kdo_pushGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *buffer, void *data, VkDeviceSize dataSize)
{
	VkResult        returnCode;

	if (buffer->info.size < buffer->fillSize + dataSize)
		kdo_reallocGPUBuffer(device, memoryProperties, commandPool, queue, buffer, buffer->fillSize + dataSize);

	KDO_VK_ASSERT(kdo_writeGPUBuffer(device, memoryProperties, commandPool, queue, buffer, buffer->fillSize, data, dataSize));

	return (VK_SUCCESS);
}


VkResult	kdo_reallocGPUImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, VkDeviceSize newSize)
{
	Kdo_VkGPUImageBuffer	newImageBuffer;
	VkResult				returnCode;
	uint32_t				i;

	newSize	= kdo_maxSize(imageBuffer->memory.size * 2, newSize);

	KDO_VK_ASSERT(kdo_newGPUImageBuffer(device, memoryProperties, imageBuffer->memory.flags, imageBuffer->memory.waitFlags, newSize, imageBuffer->maxImageCount, &imageBuffer->info, &newImageBuffer));

	i = 0;
	while (i < imageBuffer->imageCount && imageBuffer->fillSize + imageBuffer->fillSize % imageBuffer->imageArray[i].alignment + imageBuffer->imageArray[i].size <= newSize)
	{
		KDO_VK_ASSERT(kdo_pushGPUImage(device, memoryProperties, commandPool, queue, &newImageBuffer, imageBuffer->imageArray + i));
		i++;
	}

	kdo_freeGPUImageBuffer(device, imageBuffer);
	*imageBuffer	= newImageBuffer;

	return (VK_SUCCESS);
}

VkResult	kdo_bindGPUImage(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, Kdo_VkGPUImage *image)
{
	VkImageViewCreateInfo	viewInfo;
	VkDeviceSize			alignment;
	VkResult				returnCode;

	if (imageBuffer->maxImageCount <= imageBuffer->imageCount)
	  return (VK_ERROR_NOT_PERMITTED_EXT);

	alignment	= imageBuffer->fillSize % image->alignment;
	if (imageBuffer->memory.size < imageBuffer->fillSize + alignment + image->size)
		KDO_VK_ASSERT(kdo_reallocGPUImageBuffer(device, memoryProperties, commandPool, queue, imageBuffer, imageBuffer->fillSize + alignment + image->size));

	imageBuffer->fillSize	+= alignment;
	KDO_VK_ASSERT(vkBindImageMemory(device, image->path, imageBuffer->memory.path, imageBuffer->fillSize));

	viewInfo.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext                              = NULL;
	viewInfo.flags                              = 0;
	viewInfo.image                              = image->path;
	viewInfo.viewType                           = (VkImageViewType)imageBuffer->info.imageType;
	viewInfo.format                             = imageBuffer->info.format;
	viewInfo.components.r                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a                       = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask        = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel      = 0;
	viewInfo.subresourceRange.levelCount        = VK_REMAINING_MIP_LEVELS;
	viewInfo.subresourceRange.baseArrayLayer    = 0;
	viewInfo.subresourceRange.layerCount        = VK_REMAINING_ARRAY_LAYERS;
	KDO_VK_ASSERT(vkCreateImageView(device, &viewInfo, NULL, &image->view));

	imageBuffer->imageArray[imageBuffer->imageCount]	= *image;
	imageBuffer->fillSize								+= image->size;
	imageBuffer->imageCount++;

	return (VK_SUCCESS);
}

VkResult	kdo_pushGPUImage(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, Kdo_VkGPUImage *image)
{
	Kdo_VkGPUImage		newImage;
	VkCommandBuffer		commandBuffer;
	VkImageCopy			copyInfo;   
	VkResult			returnCode;

	imageBuffer->info.extent	= image->extent;
	KDO_VK_ASSERT(kdo_newGPUImage(device, image->layout, &imageBuffer->info, &newImage));
	KDO_VK_ASSERT(kdo_bindGPUImage(device, memoryProperties, commandPool, queue, imageBuffer, &newImage));

	kdo_beginUniqueCommand(device, commandPool, &commandBuffer);

	kdo_cmdImageBarrier(commandBuffer, newImage.path, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, imageBuffer->info.initialLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	kdo_cmdImageBarrier(commandBuffer, image->path, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, image->layout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	copyInfo.srcSubresource.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
	copyInfo.srcSubresource.mipLevel        = 0;
	copyInfo.srcSubresource.baseArrayLayer  = 0;
	copyInfo.srcSubresource.layerCount      = 1;
	copyInfo.srcOffset.x                    = 0;
	copyInfo.srcOffset.y                    = 0;
	copyInfo.srcOffset.z                    = 0;
	copyInfo.dstSubresource.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
	copyInfo.dstSubresource.mipLevel        = 0;
	copyInfo.dstSubresource.baseArrayLayer  = 0;
	copyInfo.dstSubresource.layerCount      = 1;
	copyInfo.dstOffset.x                    = 0;
	copyInfo.dstOffset.y                    = 0;
	copyInfo.dstOffset.z                    = 0;
	copyInfo.extent                         = image->extent;
	vkCmdCopyImage(commandBuffer, image->path, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, newImage.path, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

	kdo_cmdImageBarrier(commandBuffer, newImage.path, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image->layout);
	kdo_cmdImageBarrier(commandBuffer, image->path, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image->layout);

	kdo_endUniqueCommand(&commandBuffer, device, commandPool, queue);

	return (VK_SUCCESS);
}

VkResult	kdo_pushGPUImageFromGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, VkExtent3D extent, VkImageLayout layout, Kdo_VkGPUBuffer *buffer, VkDeviceSize offset)
{
	Kdo_VkGPUImage			newImage;
	VkCommandBuffer         commandBuffer;
	VkBufferImageCopy       copyBufferInfo;
	VkResult			returnCode;

	imageBuffer->info.extent	= extent;
	KDO_VK_ASSERT(kdo_newGPUImage(device, layout, &imageBuffer->info, &newImage));
	KDO_VK_ASSERT(kdo_bindGPUImage(device, memoryProperties, commandPool, queue, imageBuffer, &newImage));

	kdo_beginUniqueCommand(device, commandPool, &commandBuffer);

	kdo_cmdImageBarrier(commandBuffer, imageBuffer->imageArray[imageBuffer->imageCount - 1].path, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, imageBuffer->info.initialLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferInfo.bufferRowLength                  = 0;
	copyBufferInfo.bufferImageHeight                = 0;
	copyBufferInfo.imageSubresource.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
	copyBufferInfo.imageSubresource.mipLevel        = 0;
	copyBufferInfo.imageSubresource.baseArrayLayer  = 0;
	copyBufferInfo.imageSubresource.layerCount      = 1;
	copyBufferInfo.imageOffset.x                    = 0;
	copyBufferInfo.imageOffset.y                    = 0;
	copyBufferInfo.imageOffset.z                    = 0;
	copyBufferInfo.bufferOffset                     = offset;
	copyBufferInfo.imageExtent                      = extent;
	vkCmdCopyBufferToImage(commandBuffer, buffer->path, imageBuffer->imageArray[imageBuffer->imageCount - 1].path, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyBufferInfo);

	kdo_cmdImageBarrier(commandBuffer, imageBuffer->imageArray[imageBuffer->imageCount - 1].path, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout);

	kdo_endUniqueCommand(&commandBuffer, device, commandPool, queue);

	return (VK_SUCCESS);
}


VkResult	kdo_reallocCPUBuffer(Kdo_VkCPUBuffer *buffer, VkDeviceSize newSize)
{
	newSize	= kdo_maxSize(buffer->size * 2, newSize);
	if (!(buffer->path	= realloc(buffer->path, newSize)))
		return (VK_ERROR_OUT_OF_HOST_MEMORY);
	buffer->size		= newSize;
	buffer->fillSize	= kdo_minSize(buffer->fillSize, newSize);

	return (VK_SUCCESS);
}

VkResult	kdo_writeCPUBufferData(Kdo_VkCPUBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize)
{
	if (buffer->size < offset + dataSize)
		return (VK_ERROR_MEMORY_MAP_FAILED);

	if (buffer->fillSize < offset)
		return (VK_ERROR_NOT_PERMITTED_EXT);

	memcpy(buffer->path + offset, data, dataSize);

	buffer->fillSize	= kdo_maxSize(buffer->fillSize, offset + dataSize);

	return (VK_SUCCESS);
}

VkResult	kdo_pushCPUBufferData(Kdo_VkCPUBuffer *buffer, void *data, VkDeviceSize dataSize)
{
	VkResult	returnCode;

	if (buffer->size < buffer->fillSize + dataSize)
		KDO_VK_ASSERT(kdo_reallocCPUBuffer(buffer, buffer->fillSize + dataSize));

	KDO_VK_ASSERT(kdo_writeCPUBufferData(buffer, buffer->fillSize, data, dataSize));

	return (VK_SUCCESS);
}

VkResult	kdo_pushCPUImageBufferPath(Kdo_VkCPUImageBuffer *imageBuffer, char *imagePath)
{
	if (imageBuffer->maxImageCount <= imageBuffer->imageCount)
		return (VK_ERROR_NOT_PERMITTED_EXT);

	imageBuffer->path[imageBuffer->imageCount]	= imagePath;
	imageBuffer->imageCount++;

	return (VK_SUCCESS);
}


Kdo_VkBSTNode   **kdo_BSTNodeFind(Kdo_VkBSTNode **root, void *buffer, void *data, VkDeviceSize dataSize)
{
	int	cmp;

	if (*root)
	{
		cmp = memcmp(buffer + (*root)->index * dataSize, data, dataSize);
		if (0 < cmp)
			return (kdo_BSTNodeFind(&(*root)->left, buffer, data, dataSize));
		else if (cmp < 0)
			return (kdo_BSTNodeFind(&(*root)->right, buffer, data, dataSize));
	}
	return (root);
}

Kdo_VkBSTNode   **kdo_BSTNodeImageFind(Kdo_VkBSTNode **root, char *imagePath, char **pathBuffer)
{
	int	cmp;

	if (*root)
	{
		cmp = strcmp(basename(pathBuffer[(*root)->index]), basename(imagePath));
		if (0 < cmp)
			return (kdo_BSTNodeImageFind(&(*root)->left, imagePath, pathBuffer));
		else if (cmp < 0)
			return (kdo_BSTNodeImageFind(&(*root)->right, imagePath, pathBuffer));
	}
	return (root);
}

VkResult	kdo_BSTPush(Kdo_VkBST *bst, void *data, uint32_t *index)
{
	Kdo_VkBSTNode	**node;
	VkResult		returnCode;

	node = kdo_BSTNodeFind(&bst->root, bst->CPUBuffer->path, data, bst->elementSize);
	if (*node == NULL)
	{
		KDO_VK_ASSERT(kdo_newBSTNode(bst->CPUBuffer->fillSize / bst->elementSize, node));
		KDO_VK_ASSERT(kdo_pushCPUBufferData(bst->CPUBuffer, data, bst->elementSize));
	}
	*index	= (*node)->index;

	return (VK_SUCCESS);
}

VkResult	kdo_BSTImagePush(Kdo_VkImageBST *imageBst, char *imagePath, uint32_t *index)
{
	Kdo_VkBSTNode	**node;
	VkResult		returnCode;

	node = kdo_BSTNodeImageFind(&imageBst->root, imagePath, imageBst->CPUImageBuffer->path);
	if (*node == NULL)
	{
		KDO_VK_ASSERT(kdo_newBSTNode(imageBst->CPUImageBuffer->imageCount, node));
		KDO_VK_ASSERT(kdo_pushCPUImageBufferPath(imageBst->CPUImageBuffer, imagePath));
	}
	*index	= (*node)->index;

	return (VK_SUCCESS);
}


VkResult	kdo_writeBufferData(Kdo_VkBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize)
{
	return (kdo_writeCPUBufferData(&buffer->CPU, offset, data, dataSize));
}

VkResult	kdo_pushBufferData(Kdo_VkBuffer *buffer, void *data, VkDeviceSize dataSize)
{
	return (kdo_pushCPUBufferData(&buffer->CPU, data, dataSize));
}

VkResult	kdo_pushSetBufferData(Kdo_VkSetBuffer *buffer, void *data, uint32_t *index)
{
	return (kdo_BSTPush(&buffer->BST, data, index));
}

VkResult	kdo_pushSetImageBufferPath(Kdo_VkSetImageBuffer *imageBuffer, char *imagePath, uint32_t *index)
{
	int	indexNull;

	*index	= 0;
	if (stbi_info(imagePath, &indexNull, &indexNull, &indexNull))
		return (kdo_BSTImagePush(&imageBuffer->BST, imagePath, index));
	
	return (VK_SUCCESS);
}


VkResult	kdo_updateBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkBuffer *buffer, VkDeviceSize offset, VkDeviceSize size)
{
	VkResult	returnCode;

	if (buffer->CPU.size < offset + size)
		return (VK_ERROR_MEMORY_MAP_FAILED);
	if (buffer->CPU.fillSize < offset + size || buffer->GPU.fillSize < offset)
		return (VK_ERROR_NOT_PERMITTED_EXT);

	if (buffer->GPU.memory.size < offset + size)
		KDO_VK_ASSERT(kdo_reallocGPUBuffer(device, memoryProperties, commandPool, queue, &buffer->GPU, offset + size));

	KDO_VK_ASSERT(kdo_writeGPUBuffer(device, memoryProperties, commandPool, queue, &buffer->GPU, offset, buffer->CPU.path + offset, size));

	return (VK_SUCCESS);
}

VkResult	kdo_updateAllBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkBuffer *buffer)
{
	VkResult	returnCode;

	if (buffer->GPU.memory.size < buffer->CPU.fillSize)
		KDO_VK_ASSERT(kdo_reallocGPUBuffer(device, memoryProperties, commandPool, queue, &buffer->GPU, buffer->CPU.fillSize));

	KDO_VK_ASSERT(kdo_writeGPUBuffer(device, memoryProperties, commandPool, queue, &buffer->GPU, 0, buffer->CPU.path, buffer->CPU.fillSize));

	return (VK_SUCCESS);
}

VkResult	kdo_updateSetBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkSetBuffer *buffer)
{
	VkResult	returnCode;

	if (buffer->CPU.fillSize < buffer->GPU.fillSize)
		return (VK_ERROR_MEMORY_MAP_FAILED);

	KDO_VK_ASSERT(kdo_pushGPUBuffer(device, memoryProperties, commandPool, queue, &buffer->GPU, buffer->CPU.path + buffer->GPU.fillSize, buffer->CPU.fillSize - buffer->GPU.fillSize));

	return (VK_SUCCESS);
}

VkResult	kdo_updateSetImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkSetImageBuffer *imageBuffer, VkImageLayout layout)
{
	Kdo_VkGPUBuffer	stagingBuffer;
	VkResult		returnCode;
	VkExtent3D		extent;
	int				texChannels;
	void			*data;

	if (imageBuffer->CPU.imageCount < imageBuffer->GPU.imageCount)
		return (VK_ERROR_MEMORY_MAP_FAILED);

	extent.depth	= 1;
	for (uint32_t i = imageBuffer->GPU.imageCount; i < imageBuffer->CPU.imageCount; i++)
	{
		if (!(data	= stbi_load(imageBuffer->CPU.path[i], (int *)&extent.width, (int *)&extent.height, &texChannels, STBI_rgb_alpha)))
			return (VK_ERROR_UNKNOWN);

		KDO_VK_ASSERT(kdo_newGPUStagingBuffer(device, memoryProperties, &stagingBuffer, data, extent.width * extent.height * 4));
		KDO_VK_ASSERT(kdo_pushGPUImageFromGPUBuffer(device, memoryProperties, commandPool, queue, &imageBuffer->GPU, extent, layout, &stagingBuffer, 0));

		kdo_freeGPUBuffer(device, &stagingBuffer);
		stbi_image_free(data);
	}

	return (VK_SUCCESS);
}
