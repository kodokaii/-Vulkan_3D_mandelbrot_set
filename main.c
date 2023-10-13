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

#include "kdo_VkGlfw.h"
#include "kdo_VkInit.h"
#include "kdo_VkDisplay.h"

int	main(int argc, char *argv[])
{
	Kdo_Vulkan		vk						= {};
	const char		*validationLayers[]		= {"VK_LAYER_KHRONOS_validation"};
	const char		*deviceExtensions[]		= {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	(void) argc;
	(void) argv;

	vk.info.applicationName						= "Vulkan Project";
	vk.info.engineName							= "Kodo Engine";
	vk.info.instanceExtensionsCount				= 0;
	vk.info.instanceExtensions					= NULL;
	vk.info.validationLayersCount				= 1;
	vk.info.validationLayers					= validationLayers;
	vk.info.deviceExtensionsCount				= 1;
	vk.info.deviceExtensions					= deviceExtensions;
	vk.info.windowWidth							= 1500;
	vk.info.windowHeight						= 1500;

	vk.info.queuesInfo[PRESENT_QUEUE].count				= 1;
	vk.info.queuesInfo[PRESENT_QUEUE].priority			= 1.0f;
	vk.info.queuesInfo[PRESENT_QUEUE].requiredFlags		= 0; 
	vk.info.queuesInfo[PRESENT_QUEUE].noRequiredFlags	= 0;
	vk.info.queuesInfo[PRESENT_QUEUE].preferredFlags	= VK_QUEUE_GRAPHICS_BIT;
	vk.info.queuesInfo[PRESENT_QUEUE].noPreferredFlags	= 0;
	vk.info.queuesInfo[PRESENT_QUEUE].presentSupport	= VK_TRUE;

	vk.info.queuesInfo[GRAPHIC_QUEUE].count				= 1;
	vk.info.queuesInfo[GRAPHIC_QUEUE].priority			= 1.0f;
	vk.info.queuesInfo[GRAPHIC_QUEUE].requiredFlags		= VK_QUEUE_GRAPHICS_BIT;
	vk.info.queuesInfo[GRAPHIC_QUEUE].noRequiredFlags	= 0;
	vk.info.queuesInfo[GRAPHIC_QUEUE].preferredFlags	= 0;
	vk.info.queuesInfo[GRAPHIC_QUEUE].noPreferredFlags	= 0;
	vk.info.queuesInfo[GRAPHIC_QUEUE].presentSupport	= 0;

	vk.info.queuesInfo[TRANSFER_QUEUE].count			= 1;
	vk.info.queuesInfo[TRANSFER_QUEUE].priority			= 1.0f;
	vk.info.queuesInfo[TRANSFER_QUEUE].requiredFlags	= VK_QUEUE_GRAPHICS_BIT;
	vk.info.queuesInfo[TRANSFER_QUEUE].noRequiredFlags	= 0;
	vk.info.queuesInfo[TRANSFER_QUEUE].preferredFlags	= 0;
	vk.info.queuesInfo[TRANSFER_QUEUE].noPreferredFlags	= 0;
	vk.info.queuesInfo[TRANSFER_QUEUE].presentSupport	= 0;

	vk.info.presentMode							= VK_PRESENT_MODE_MAILBOX_KHR;
	vk.info.startCenter[0]						= -0.7484789953193998;
	vk.info.startCenter[1]						= 0.07591647038028744f;
	vk.info.startCenter[2]						= 0.0f;
	vk.info.startRadius							= 2.0f;
	vk.info.startCubeSize						= 100;
	vk.info.startIterMax						= 20;

	kdo_initGlfw(&vk);
	kdo_initVulkan(&vk);
	kdo_mainLoop(&vk);

	kdo_cleanup(&vk, "Good !", 0);
}
