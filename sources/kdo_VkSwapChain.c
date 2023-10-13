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

#include "kdo_VkSwapChain.h"

static VkExtent2D	kdo_chooseSwapExtent(Kdo_Vulkan *vk)
{
	VkExtent2D	extent;

	if (vk->physicalDevice.swapChainProperties.capabilities.currentExtent.width != UINT32_MAX)
			return (vk->physicalDevice.swapChainProperties.capabilities.currentExtent);

	glfwGetFramebufferSize(vk->window.path, (int *) &extent.width, (int *) &extent.height);
	while (extent.width == 0 || extent.height == 0)
	{
		glfwWaitEvents();
		glfwGetFramebufferSize(vk->window.path, (int *) &extent.width, (int *) &extent.height);
	}

	if (extent.width < vk->physicalDevice.swapChainProperties.capabilities.minImageExtent.width)
		extent.width = vk->physicalDevice.swapChainProperties.capabilities.minImageExtent.width;
	else if (vk->physicalDevice.swapChainProperties.capabilities.maxImageExtent.width < extent.width)
		extent.width = vk->physicalDevice.swapChainProperties.capabilities.maxImageExtent.width;

	if (extent.height < vk->physicalDevice.swapChainProperties.capabilities.minImageExtent.height)
		extent.height = vk->physicalDevice.swapChainProperties.capabilities.minImageExtent.height;
	else if (vk->physicalDevice.swapChainProperties.capabilities.maxImageExtent.height < extent.height)
		extent.height = vk->physicalDevice.swapChainProperties.capabilities.maxImageExtent.height;

	return (extent);
}

static uint32_t	kdo_chooseSwapImagesCount(Kdo_Vulkan *vk)
{
	uint32_t imageCount = vk->physicalDevice.swapChainProperties.capabilities.minImageCount + 1;

	if (0 < vk->physicalDevice.swapChainProperties.capabilities.maxImageCount && vk->physicalDevice.swapChainProperties.capabilities.maxImageCount < imageCount)
		imageCount = vk->physicalDevice.swapChainProperties.capabilities.maxImageCount;
	return (imageCount);
}

static VkSurfaceFormatKHR	kdo_chooseSwapSurfaceFormat(Kdo_Vulkan *vk)
{
	uint32_t	i;

	for (i = 0; i < vk->physicalDevice.swapChainProperties.formatsCount && (vk->physicalDevice.swapChainProperties.formats[i].format != VK_FORMAT_B8G8R8A8_SRGB || vk->physicalDevice.swapChainProperties.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR); i++);
	if (i < vk->physicalDevice.swapChainProperties.formatsCount)
		return (vk->physicalDevice.swapChainProperties.formats[i]);
	return (vk->physicalDevice.swapChainProperties.formats[0]);
}

static VkPresentModeKHR	kdo_chooseSwapPresentMode(Kdo_Vulkan *vk)
{
	uint32_t	i;

		for (i = 0; i < vk->physicalDevice.swapChainProperties.presentModesCount && vk->physicalDevice.swapChainProperties.presentModes[i] != vk->info.presentMode; i++);
	if (i < vk->physicalDevice.swapChainProperties.presentModesCount)
		return (vk->physicalDevice.swapChainProperties.presentModes[vk->physicalDevice.swapChainProperties.presentModesCount]);
	return (VK_PRESENT_MODE_FIFO_KHR);
}

void	kdo_initViewsSwapChain(Kdo_Vulkan *vk)
{
	VkImageViewCreateInfo		viewInfo;

	vkGetSwapchainImagesKHR(vk->device.path, vk->swapChain.path, &vk->swapChain.imagesCount, NULL);
	KDO_VK_ALLOC(vk->swapChain.images, malloc(vk->swapChain.imagesCount * sizeof(VkImage)));
	KDO_VK_ALLOC(vk->swapChain.views, malloc(vk->swapChain.imagesCount * sizeof(VkImageView)));
	vkGetSwapchainImagesKHR(vk->device.path, vk->swapChain.path, &vk->swapChain.imagesCount, vk->swapChain.images);


	for (uint32_t i = 0; i < vk->swapChain.imagesCount; i++)
	{
		viewInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext								= NULL;
		viewInfo.flags								= 0;
		viewInfo.image								= vk->swapChain.images[i];
		viewInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format								= vk->swapChain.imagesFormat.format;
		viewInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel		= 0;
		viewInfo.subresourceRange.levelCount		= 1;
		viewInfo.subresourceRange.baseArrayLayer	= 0;
		viewInfo.subresourceRange.layerCount		= 1;
		if (vkCreateImageView(vk->device.path, &viewInfo, NULL, vk->swapChain.views + i) != VK_SUCCESS)
			kdo_cleanup(vk, "Views SwapChain creation failed", 9);
	}
}

void	kdo_initSwapChain(Kdo_Vulkan *vk)
{
	VkSwapchainCreateInfoKHR	swapChainInfo;

	vk->swapChain.imagesExtent = kdo_chooseSwapExtent(vk);
	vk->swapChain.imagesFormat = kdo_chooseSwapSurfaceFormat(vk);

	swapChainInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.pNext					= NULL;
	swapChainInfo.flags					= 0;
	swapChainInfo.surface				= vk->surface.path;
	swapChainInfo.minImageCount			= kdo_chooseSwapImagesCount(vk);
	swapChainInfo.imageFormat			= vk->swapChain.imagesFormat.format;
	swapChainInfo.imageColorSpace		= vk->swapChain.imagesFormat.colorSpace;
	swapChainInfo.imageExtent			= vk->swapChain.imagesExtent;
	swapChainInfo.imageArrayLayers		= 1;
	swapChainInfo.imageUsage			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainInfo.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	swapChainInfo.queueFamilyIndexCount = 0;
	swapChainInfo.pQueueFamilyIndices	= 0;
	swapChainInfo.preTransform			= vk->physicalDevice.swapChainProperties.capabilities.currentTransform;
	swapChainInfo.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainInfo.presentMode			= kdo_chooseSwapPresentMode(vk);
	swapChainInfo.clipped				= VK_TRUE;
	swapChainInfo.oldSwapchain			= NULL;
	if (vkCreateSwapchainKHR(vk->device.path, &swapChainInfo, NULL, &vk->swapChain.path) != VK_SUCCESS)
		kdo_cleanup(vk, "SwapChain creation failed", 8);

	kdo_freePhysicalDevice(vk);
}

void	kdo_recreateSwapchain(Kdo_Vulkan *vk)
{
	vkDeviceWaitIdle(vk->device.path);
	kdo_swapChainCleanup(vk);
	
	kdo_getSwapChainProperties(vk);
	kdo_initSwapChain(vk);
	kdo_initViewsSwapChain(vk);
	kdo_initRenderPass(vk);
	kdo_initFramebuffers(vk);
	kdo_initSyncronisation(vk);
	kdo_initRenderPool(vk);

	vk->display.windowResized = 0;
}
