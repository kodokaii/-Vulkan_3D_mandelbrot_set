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

#ifndef KDO_VKMEMORY_H
# define KDO_VKMEMORY_H

# include "kdo_Vulkan.h"

void		kdo_vkNewBuffer(Kdo_Vulkan *vk, VkMemoryPropertyFlags memoryFlags, VkBufferUsageFlags usage, VkDeviceSize size, Kdo_VkBuffer *buffer);
void		kdo_vkNewSetBuffer(Kdo_Vulkan *vk, VkMemoryPropertyFlags memoryFlags, VkBufferUsageFlags usage, VkDeviceSize elementSize, VkDeviceSize size, Kdo_VkSetBuffer *buffer);
void		kdo_vkNewSetImageBuffer(Kdo_Vulkan *vk, VkMemoryPropertyFlags memoryFlags, uint32_t maxImageCount, VkDeviceSize size, Kdo_VkSetImageBuffer *imageBuffer);

void		kdo_vkFreeBuffer(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer);
void		kdo_vkFreeSetBuffer(Kdo_Vulkan *vk, Kdo_VkSetBuffer *buffer);
void		kdo_vkFreeSetImageBuffer(Kdo_Vulkan *vk, Kdo_VkSetImageBuffer *imageBuffer);

void		kdo_vkWriteBufferData(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer, VkDeviceSize offset, void *data, VkDeviceSize dataSize);
void		kdo_vkPushBufferData(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer, void *data, VkDeviceSize dataSize);
void		kdo_vkPushSetBufferData(Kdo_Vulkan *vk, Kdo_VkSetBuffer *buffer, void *data, uint32_t *index);
void		kdo_vkPushSetImageBufferPath(Kdo_Vulkan *vk, Kdo_VkSetImageBuffer *imageBuffer, char *path, uint32_t *index);

void		kdo_vkBeginUniqueCommand(Kdo_Vulkan *vk, VkCommandBuffer *commandBuffer);
uint32_t    kdo_vkFindMemoryType(Kdo_Vulkan *vk, uint32_t typeFilter, VkMemoryPropertyFlags memoryFlags);
void		kdo_vkEndUniqueCommand(Kdo_Vulkan *vk, VkCommandBuffer *commandBuffer);

void		kdo_vkUpdateBuffer(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer, VkDeviceSize offset, VkDeviceSize size);
void		kdo_vkUpdateAllBuffer(Kdo_Vulkan *vk, Kdo_VkBuffer *buffer);
void		kdo_vkUpdateSetBuffer(Kdo_Vulkan *vk, Kdo_VkSetBuffer *buffer);
void		kdo_vkUpdateSetImageBuffer(Kdo_Vulkan *vk, Kdo_VkSetImageBuffer *imageBuffer, VkImageLayout layout);

#endif
