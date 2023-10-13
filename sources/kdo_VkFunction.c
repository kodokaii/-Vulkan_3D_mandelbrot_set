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
#include "kdo_VkFunction.h"

int	kdo_checkExtensions(const char **find, uint32_t findCount, const char *list, uint32_t listCount, size_t offsetList)
{
	uint32_t	j;

	for (uint32_t i = 0; i < findCount; i++)
	{
		for (j = 0; j < listCount && strcmp(find[i], list + j * offsetList); j++);
		if (j == listCount)
		{
			printf("%s is missing\n", find[i]);
			return (1);
		}
	}
	return (0);
}

VkFormat	kdo_findFormat(Kdo_Vulkan *vk, VkFormat *formats, uint32_t formatsCount, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	VkFormatProperties	properties;

	for (uint32_t i = 0; i < formatsCount; i++)
	{
		vkGetPhysicalDeviceFormatProperties(vk->physicalDevice.path, formats[i], &properties);
		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			return (formats[i]);
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			return (formats[i]);
	}
	kdo_cleanup(vk, "Couldn't find format", 8);
	return (8);
}
