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

#include "kdo_VkPhysicalDevice.h"

void	kdo_getSwapChainProperties(Kdo_Vulkan *vk)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk->physicalDevice.path, vk->surface.path, &vk->physicalDevice.swapChainProperties.capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physicalDevice.path, vk->surface.path, &vk->physicalDevice.swapChainProperties.formatsCount, NULL);
	KDO_VK_ALLOC(vk->physicalDevice.swapChainProperties.formats, malloc(vk->physicalDevice.swapChainProperties.formatsCount * sizeof(VkSurfaceFormatKHR)));
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physicalDevice.path, vk->surface.path, &vk->physicalDevice.swapChainProperties.formatsCount, vk->physicalDevice.swapChainProperties.formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physicalDevice.path, vk->surface.path, &vk->physicalDevice.swapChainProperties.presentModesCount, NULL);
	KDO_VK_ALLOC(vk->physicalDevice.swapChainProperties.presentModes, malloc(vk->physicalDevice.swapChainProperties.presentModesCount * sizeof(VkPresentInfoKHR)));
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physicalDevice.path, vk->surface.path, &vk->physicalDevice.swapChainProperties.presentModesCount, vk->physicalDevice.swapChainProperties.presentModes);
}

void	kdo_getQueueProperties(Kdo_Vulkan *vk)
{
	vkGetPhysicalDeviceQueueFamilyProperties(vk->physicalDevice.path, &vk->physicalDevice.queueProperties.famillesCount, NULL);
	KDO_VK_ALLOC(vk->physicalDevice.queueProperties.path, malloc(vk->physicalDevice.queueProperties.famillesCount * sizeof(VkQueueFamilyProperties)));
	vkGetPhysicalDeviceQueueFamilyProperties(vk->physicalDevice.path, &vk->physicalDevice.queueProperties.famillesCount, vk->physicalDevice.queueProperties.path);

	KDO_VK_ALLOC(vk->physicalDevice.queueProperties.presentSupport, malloc(vk->physicalDevice.queueProperties.famillesCount * sizeof(VkBool32)));
	for (uint32_t i = 0; i < vk->physicalDevice.queueProperties.famillesCount; i++)
		vkGetPhysicalDeviceSurfaceSupportKHR(vk->physicalDevice.path, i, vk->surface.path, vk->physicalDevice.queueProperties.presentSupport + i);
}

static int	kdo_checkPhysicalDevice(Kdo_Vulkan *vk)
{
	VkExtensionProperties       *availableExtensions;
	uint32_t                    extensionCount;
	uint32_t					i;
	VkQueueFlags				queueRequiredFlags	= vk->info.queuesInfo[PRESENT_QUEUE].requiredFlags | vk->info.queuesInfo[GRAPHIC_QUEUE].requiredFlags| vk->info.queuesInfo[TRANSFER_QUEUE].requiredFlags;
	VkBool32					presentSupport	= 0;

	vkEnumerateDeviceExtensionProperties(vk->physicalDevice.path, NULL, &extensionCount, NULL);
	KDO_VK_ALLOC(availableExtensions, malloc(extensionCount * sizeof(VkExtensionProperties)));
	vkEnumerateDeviceExtensionProperties(vk->physicalDevice.path, NULL, &extensionCount, availableExtensions);
	if (kdo_checkExtensions(vk->info.deviceExtensions, vk->info.deviceExtensionsCount, (const char *) availableExtensions->extensionName, extensionCount, sizeof(VkExtensionProperties)))
		return (1);

	kdo_getSwapChainProperties(vk);
	if (!vk->physicalDevice.swapChainProperties.formats->format || !*vk->physicalDevice.swapChainProperties.presentModes)
          return (1);

	kdo_getQueueProperties(vk);
	for (i = 0; i < vk->physicalDevice.queueProperties.famillesCount && (queueRequiredFlags || !presentSupport); i++)
	{
		if (vk->physicalDevice.queueProperties.presentSupport[i])
			presentSupport = 1;
		queueRequiredFlags ^= vk->physicalDevice.queueProperties.path[i].queueFlags & queueRequiredFlags;
	}
	if (i == vk->physicalDevice.queueProperties.famillesCount)
		return (1);

	vkGetPhysicalDeviceProperties(vk->physicalDevice.path, &vk->physicalDevice.properties);
	vkGetPhysicalDeviceFeatures(vk->physicalDevice.path, &vk->physicalDevice.features);
	vkGetPhysicalDeviceMemoryProperties(vk->physicalDevice.path, &vk->physicalDevice.memoryProperties);
	if(!vk->physicalDevice.features.geometryShader || !vk->physicalDevice.features.shaderFloat64 || vk->physicalDevice.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		return (1);

	return (0);
}

void	kdo_initPhysicalDevice(Kdo_Vulkan *vk)
{
	VkPhysicalDevice            *physicalDevices;
	uint32_t                    physicalDeviceCount;

	vkEnumeratePhysicalDevices(vk->instance.path, &physicalDeviceCount, NULL);
	if (!physicalDeviceCount)
		kdo_cleanup(vk, "No graphics card supports Vulkan", 4);
	KDO_VK_ALLOC(physicalDevices, malloc(physicalDeviceCount * sizeof(VkPhysicalDevice)));
	vkEnumeratePhysicalDevices(vk->instance.path, &physicalDeviceCount, physicalDevices);

	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		vk->physicalDevice.path = physicalDevices[i];
		if (kdo_checkPhysicalDevice(vk))
			kdo_freePhysicalDevice(vk);
		else
		{
			free(physicalDevices);
			return;
		}
	}
	kdo_cleanup(vk, "Couldn't find a suitable GPU", 5);
}
