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

#ifndef KDO_VKBUFFER_H
# define KDO_VKBUFFER_H

# include <vulkan/vulkan.h>
# include <libgen.h>

typedef enum Kdo_VkWait
{
	WAIT_NONE	= 0x00000000,
	WAIT_DEVICE	= 0x00000001
}	Kdo_VkWait;

typedef struct Kdo_VkGPUMemory
{
    VkDeviceMemory			path;
	VkMemoryPropertyFlags	flags;
	Kdo_VkWait				waitFlags;
    VkDeviceSize			size;
}	Kdo_VkGPUMemory;

typedef struct Kdo_VkGPUBuffer
{
    VkBuffer			path;
	Kdo_VkGPUMemory		memory;
	VkBufferCreateInfo	info;
	VkDeviceSize		fillSize;
}	Kdo_VkGPUBuffer;
typedef struct Kdo_VkGPUImage
{
    VkImage			path;
    VkImageView		view;
    VkExtent3D		extent;
    VkImageLayout	layout;
    VkDeviceSize	alignment;
	VkDeviceSize	size;
}	Kdo_VkGPUImage;

typedef struct Kdo_VkGPUImageBuffer
{
    Kdo_VkGPUImage			*imageArray;
    Kdo_VkGPUMemory			memory;
	VkImageCreateInfo		info;
	VkDeviceSize			fillSize;
	uint32_t				maxImageCount;
	uint32_t				imageCount;
}	Kdo_VkGPUImageBuffer;

typedef struct Kdo_VkCPUBuffer
{
    void			*path;
	VkDeviceSize	size;
	VkDeviceSize	fillSize;
}	Kdo_VkCPUBuffer;

typedef struct Kdo_VkCPUImageBuffer
{
    char		**path;
	uint32_t	maxImageCount;
	uint32_t	imageCount;
}	Kdo_VkCPUImageBuffer;


typedef struct Kdo_VkBSTNode
{
    struct Kdo_VkBSTNode	*left;
    struct Kdo_VkBSTNode	*right;
    uint32_t				index;
}   Kdo_VkBSTNode;

typedef struct Kdo_VkBST
{
	Kdo_VkBSTNode	*root;
	Kdo_VkCPUBuffer	*CPUBuffer;
	VkDeviceSize	elementSize;
}	Kdo_VkBST;

typedef struct Kdo_VkImageBST
{
	Kdo_VkBSTNode			*root;
	Kdo_VkCPUImageBuffer	*CPUImageBuffer;
}	Kdo_VkImageBST;


typedef	struct Kdo_VkBuffer
{
    Kdo_VkGPUBuffer	GPU;
    Kdo_VkCPUBuffer	CPU;
}	Kdo_VkBuffer;

typedef struct Kdo_VkSetBuffer
{
    Kdo_VkGPUBuffer	GPU;
    Kdo_VkCPUBuffer	CPU;
	Kdo_VkBST		BST;
}	Kdo_VkSetBuffer;

typedef struct	Kdo_VkSetImageBuffer
{
    Kdo_VkGPUImageBuffer	GPU;
    Kdo_VkCPUImageBuffer	CPU;
    Kdo_VkImageBST			BST;
}	Kdo_VkSetImageBuffer;

# define KDO_VK_ASSERT(function) if ((returnCode = function) != VK_SUCCESS) return (returnCode)
# define KDO_DESTROY(function, device, handle) if (handle) {function(device, handle, NULL); handle = VK_NULL_HANDLE;}
# define KDO_FREE(pointer) if (pointer != NULL) {free(pointer); pointer = NULL;}

# define kdo_getGPUBuffer(buffer)				buffer.GPU.path
# define kdo_getGPUBufferSize(buffer)			buffer.GPU.fillSize
# define kdo_getCPUBuffer(buffer)				buffer.CPU.path
# define kdo_getCPUBufferSize(buffer)			buffer.CPU.fillSize
# define kdo_getCPUImageBufferCount(buffer)		buffer.CPU.imageCount
# define kdo_getGPUImage(imageBuffer, index)	imageBuffer.GPU.imageArray[index]
# define kdo_getGPULastImage(imageBuffer)		imageBuffer.GPU.imageArray[imageBuffer.GPU.imageCount - 1]
# define kdo_getGPUImageCount(imageBuffer)		imageBuffer.GPU.imageCount

VkResult		kdo_newGPUMemory(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, uint32_t memoryFilter, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize size, Kdo_VkGPUMemory *memory);
VkResult		kdo_newGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkBufferCreateInfo *bufferInfo, Kdo_VkGPUBuffer *buffer);
VkResult		kdo_newGPUStagingBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, Kdo_VkGPUBuffer *stagingBuffer, void *data, VkDeviceSize dataSize);
VkResult		kdo_newGPUImage(VkDevice device, VkImageLayout layout, VkImageCreateInfo *imageInfo, Kdo_VkGPUImage *image);
VkResult		kdo_newGPUImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize size, uint32_t maxImageCount, VkImageCreateInfo  *imageInfo, Kdo_VkGPUImageBuffer *imageBuffer);
VkResult		kdo_newCPUBuffer(VkDeviceSize size, Kdo_VkCPUBuffer *buffer);
VkResult        kdo_newCPUImageBuffer(uint32_t  maxImageCount, Kdo_VkCPUImageBuffer *imageBuffer);
VkResult		kdo_newBSTNode(uint32_t index, Kdo_VkBSTNode **node);
VkResult		kdo_newBST(VkDeviceSize elementSize, Kdo_VkCPUBuffer *CPUBuffer, Kdo_VkBST *bst);
VkResult        kdo_newImageBST(Kdo_VkCPUImageBuffer *CPUImageBuffer, Kdo_VkImageBST *bst);	
VkResult		kdo_newBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkBufferCreateInfo *bufferInfo, Kdo_VkBuffer *buffer);
VkResult		kdo_newSetBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize elementSize, VkBufferCreateInfo *bufferInfo, Kdo_VkSetBuffer *buffer);
VkResult		kdo_newSetImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, Kdo_VkWait waitFlags, VkDeviceSize size, uint32_t maxImageCount, VkImageCreateInfo  *imageInfo, Kdo_VkSetImageBuffer *imageBuffer);


void			kdo_freeGPUMemory(VkDevice device, Kdo_VkGPUMemory *memory);
void			kdo_freeGPUBuffer(VkDevice device, Kdo_VkGPUBuffer *buffer);
void			kdo_freeGPUImage(VkDevice device, Kdo_VkGPUImage *image);
void			kdo_freeGPUImageBuffer(VkDevice device, Kdo_VkGPUImageBuffer *imageBuffer);
void			kdo_freeCPUBuffer(Kdo_VkCPUBuffer *buffer);
void			kdo_freeCPUImageBuffer(Kdo_VkCPUImageBuffer *imageBuffer);
void			kdo_freeBSTNode(Kdo_VkBSTNode **node);
void			kdo_freeBST(Kdo_VkBST *bst);
void			kdo_freeImageBST(Kdo_VkImageBST *imageBst);
void			kdo_freeBuffer(VkDevice device, Kdo_VkBuffer *buffer);
void			kdo_freeSetBuffer(VkDevice device, Kdo_VkSetBuffer *buffer);
void			kdo_freeSetImageBuffer(VkDevice device, Kdo_VkSetImageBuffer *imageBuffer);


VkResult		kdo_beginUniqueCommand(VkDevice device, VkCommandPool commandPool, VkCommandBuffer *commandBuffer);
VkResult        kdo_findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags memoryFlags, uint32_t *memoryType);
VkResult		kdo_findMemoryFilterImage(VkDevice device, VkImageCreateInfo *imageInfo, uint32_t *memoryFilter);
VkDeviceSize	kdo_minSize(VkDeviceSize val1, VkDeviceSize val2);
VkDeviceSize	kdo_maxSize(VkDeviceSize val1, VkDeviceSize val2);
VkResult		kdo_cmdImageBarrier(VkCommandBuffer commandBuffer, VkImage image, \
								    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, \
									VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, \
									VkImageLayout oldLayout, VkImageLayout newLayout);
VkResult		kdo_endUniqueCommand(VkCommandBuffer *commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue queue);


VkResult        kdo_reallocGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *buffer, VkDeviceSize newSize);
VkResult		kdo_writeHostGPUMemory(VkDevice device, Kdo_VkGPUMemory *memory, VkDeviceSize offset, void *data, VkDeviceSize dataSize);
VkResult		kdo_writeGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize);
VkResult		kdo_cpyGPUBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *dstBuffer, VkDeviceSize dstOffset, Kdo_VkGPUBuffer *srcBuffer, VkDeviceSize srcOffset, VkDeviceSize dataSize);
VkResult		kdo_pushGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUBuffer *buffer, void *data, VkDeviceSize dataSize);
VkResult		kdo_reallocGPUImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, VkDeviceSize newSize);
VkResult		kdo_bindGPUImage(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, Kdo_VkGPUImage *image);
VkResult		kdo_pushGPUImage(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, Kdo_VkGPUImage *image);
VkResult		kdo_pushGPUImageFromGPUBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkGPUImageBuffer *imageBuffer, VkExtent3D extent, VkImageLayout layout, Kdo_VkGPUBuffer *buffer, VkDeviceSize offset);

VkResult		kdo_reallocCPUBuffer(Kdo_VkCPUBuffer *buffer, VkDeviceSize newSize);
VkResult		kdo_writeCPUBufferData(Kdo_VkCPUBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize);
VkResult		kdo_pushCPUBufferData(Kdo_VkCPUBuffer *buffer, void *data, VkDeviceSize dataSize);
VkResult	    kdo_pushCPUImageBufferPath(Kdo_VkCPUImageBuffer *imageBuffer, char *imagePath);
Kdo_VkBSTNode   **kdo_BSTNodeFind(Kdo_VkBSTNode **root, void *buffer, void *data, VkDeviceSize dataSize);
Kdo_VkBSTNode   **kdo_BSTNodeImageFind(Kdo_VkBSTNode **root, char *imagePath, char **pathBuffer);
VkResult		kdo_BSTPush(Kdo_VkBST *bst, void *data, uint32_t *index);
VkResult        kdo_BSTImagePush(Kdo_VkImageBST *imageBst, char *imagePath, uint32_t *index);
VkResult		kdo_BSTImagePush(Kdo_VkImageBST *bst, char *imagePath, uint32_t *index);

VkResult		kdo_writeBufferData(Kdo_VkBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize);
VkResult		kdo_pushBufferData(Kdo_VkBuffer *buffer, void *data, VkDeviceSize dataSize);
VkResult		kdo_pushSetBufferData(Kdo_VkSetBuffer *buffer, void *data, uint32_t *index);
VkResult        kdo_pushSetImageBufferPath(Kdo_VkSetImageBuffer *imageBuffer, char *imagePath, uint32_t *index);

VkResult		kdo_updateBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkBuffer *buffer, VkDeviceSize offset, VkDeviceSize size);
VkResult		kdo_updateAllBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkBuffer *buffer);
VkResult		kdo_updateSetBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkSetBuffer *buffer);
VkResult        kdo_updateSetImageBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties, VkCommandPool commandPool, VkQueue queue, Kdo_VkSetImageBuffer *imageBuffer, VkImageLayout layout);


#endif
