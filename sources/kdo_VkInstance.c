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

#include "kdo_VkInstance.h"

void    kdo_initInstance(Kdo_Vulkan *vk)
{
	VkExtensionProperties   *availableExtensions;
	VkLayerProperties       *availableLayers;
	uint32_t                extensionsCount;
	VkApplicationInfo       appInfo;
	VkInstanceCreateInfo    instanceInfo;

	vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, NULL);
	KDO_VK_ALLOC(availableExtensions, malloc(extensionsCount * sizeof(VkExtensionProperties)));

	vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, availableExtensions);
	if (kdo_checkExtensions(vk->info.instanceExtensions, vk->info.instanceExtensionsCount, (const char *) availableExtensions->extensionName, extensionsCount, sizeof(VkExtensionProperties)))
		kdo_cleanup(vk, "some extensions are missing", 1);
	free(availableExtensions);

	vkEnumerateInstanceLayerProperties(&extensionsCount, NULL);
	KDO_VK_ALLOC(availableLayers, malloc(extensionsCount * sizeof(VkLayerProperties)));

	vkEnumerateInstanceLayerProperties(&extensionsCount, availableLayers);
	if (kdo_checkExtensions(vk->info.validationLayers, vk->info.validationLayersCount, (const char*) availableLayers->layerName, extensionsCount, sizeof(VkLayerProperties)))
		kdo_cleanup(vk, "The validation Layers are actived but some layers are missing", 1);
	free(availableLayers);

	appInfo.sType                   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext                   = NULL;
    appInfo.pApplicationName        = vk->info.applicationName;
    appInfo.applicationVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName             = vk->info.engineName;
    appInfo.engineVersion           = VK_MAKE_VERSION(2, 0, 0);
    appInfo.apiVersion              = VK_API_VERSION_1_0;	

	instanceInfo.sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext                      = NULL;
    instanceInfo.flags                      = 0;
    instanceInfo.pApplicationInfo           = &appInfo;
    instanceInfo.enabledLayerCount          = vk->info.validationLayersCount;
    instanceInfo.ppEnabledLayerNames        = vk->info.validationLayers;
    instanceInfo.enabledExtensionCount      = vk->info.instanceExtensionsCount;
	instanceInfo.ppEnabledExtensionNames    = vk->info.instanceExtensions;
	if (vkCreateInstance(&instanceInfo, NULL, &vk->instance.path) != VK_SUCCESS)
		kdo_cleanup(vk, "Instance creation failed", 2);
}
