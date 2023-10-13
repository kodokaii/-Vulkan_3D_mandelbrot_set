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

#include "kdo_VkDevice.h"

static uint32_t	kdo_findQueueFamilyIndex(Kdo_Vulkan *vk, Kdo_VkQueueInfo info)
{
	int			scoreIndex = -1;
	int			scoreCurrent;
	uint32_t	index;
	uint32_t	current;

	for (current = 0; current < vk->physicalDevice.queueProperties.famillesCount; current++)
	{
		if (info.count <= vk->physicalDevice.queueProperties.path[current].queueCount && (info.requiredFlags & vk->physicalDevice.queueProperties.path[current].queueFlags) == info.requiredFlags && !(info.noRequiredFlags & vk->physicalDevice.queueProperties.path[current].queueFlags) && (!info.presentSupport || vk->physicalDevice.queueProperties.presentSupport[current]))
		{
			if ((info.preferredFlags & vk->physicalDevice.queueProperties.path[current].queueFlags) == info.preferredFlags && !(info.noPreferredFlags & vk->physicalDevice.queueProperties.path[current].queueFlags))
			{
				vk->physicalDevice.queueProperties.path[current].queueCount--;
				return (current);
			}
			else
			{
				scoreCurrent = 0;
				for (uint32_t i = 0; info.preferredFlags >> i || info.noPreferredFlags >> i; i++)
				{
					if ((info.preferredFlags >> i) & 1 && ((vk->physicalDevice.queueProperties.path[current].queueFlags >> i) & 1))
						scoreCurrent++;
					if ((info.noPreferredFlags >> i) & 1 && !((vk->physicalDevice.queueProperties.path[current].queueFlags >> i) & 1))
						scoreCurrent++;
				}
				if (scoreIndex < scoreCurrent)
				{
					scoreIndex = scoreCurrent;
					index = current;
				}
			}
		}
	}
	if (scoreIndex != -1)
	{
		vk->physicalDevice.queueProperties.path[index].queueCount--;
		return (index);
	}
	kdo_cleanup(vk, "Couldn't find queue family", 6);
	return (5);
}

static uint32_t	kdo_createQueueInfo(Kdo_Vulkan *vk, VkDeviceQueueCreateInfo **queueCreateInfo, float *queuePriority)
{
	uint32_t	uniqueIndexCount;
	uint32_t	maxIndex;
	uint32_t	i;
	uint32_t	j;
	uint32_t	k;
	uint32_t	l;

	for (i = 0; i < QUEUES_COUNT; i++)
		vk->device.queues[i].familyIndex = kdo_findQueueFamilyIndex(vk, vk->info.queuesInfo[i]);

	uniqueIndexCount = 0;
	for (i = 0; i < QUEUES_COUNT; i++)
	{
		if (!vk->device.queues[i].index)
			uniqueIndexCount++;
		for(j = i + 1; j < QUEUES_COUNT; j++)
		{
			if (vk->device.queues[i].familyIndex == vk->device.queues[j].familyIndex)
				vk->device.queues[j].index++;
		}
	}

	KDO_VK_ALLOC(*queueCreateInfo, malloc(uniqueIndexCount * sizeof(VkDeviceQueueCreateInfo)));
	
	l = 0;
	for (i = 0; i < QUEUES_COUNT; i++)
	{
		if (!vk->device.queues[i].index)
		{
			k = 0;
			for (j = i; j < QUEUES_COUNT; j++)
			{
				if (vk->device.queues[i].familyIndex == vk->device.queues[j].familyIndex)
				{
					maxIndex = vk->device.queues[j].index;
					queuePriority[k++] = vk->info.queuesInfo[j].priority;
				}
			}
			(*queueCreateInfo)[l].sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			(*queueCreateInfo)[l].pNext				= NULL;
			(*queueCreateInfo)[l].flags				= 0;
			(*queueCreateInfo)[l].queueFamilyIndex	= vk->device.queues[i].familyIndex;
			(*queueCreateInfo)[l].queueCount			= maxIndex + 1;
			(*queueCreateInfo)[l].pQueuePriorities	= queuePriority;
			queuePriority += k;
			l++;
		}
	}

	return (uniqueIndexCount);
}

void    kdo_initDevice(Kdo_Vulkan *vk)
{
	VkDeviceCreateInfo							deviceInfo;
	VkDeviceQueueCreateInfo						*queueCreateInfo;
	VkPhysicalDeviceFeatures					deviceFeatures					= {};
	float										queuePriority[QUEUES_COUNT];

	deviceFeatures.shaderFloat64		= 1;

	deviceInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext					= NULL;
	deviceInfo.flags					= 0;
	deviceInfo.queueCreateInfoCount		= kdo_createQueueInfo(vk, &queueCreateInfo, queuePriority); 
	deviceInfo.pQueueCreateInfos		= queueCreateInfo;
	deviceInfo.enabledLayerCount		= vk->info.validationLayersCount;
	deviceInfo.ppEnabledLayerNames		= vk->info.validationLayers;
	deviceInfo.enabledExtensionCount	= vk->info.deviceExtensionsCount;
	deviceInfo.ppEnabledExtensionNames	= vk->info.deviceExtensions;
	deviceInfo.pEnabledFeatures			= &deviceFeatures;
	if (vkCreateDevice(vk->physicalDevice.path, &deviceInfo, NULL, &vk->device.path) != VK_SUCCESS)
		kdo_cleanup(vk, "Logical device creation failed", 7);
	
	free(queueCreateInfo);

	for (uint32_t i = 0; i < QUEUES_COUNT; i++)
		vkGetDeviceQueue(vk->device.path, vk->device.queues[i].familyIndex, vk->device.queues[i].index, &vk->device.queues[i].path);
}
