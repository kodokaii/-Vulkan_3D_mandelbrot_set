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

#include "kdo_VkInit.h"

void	kdo_initVulkan(Kdo_Vulkan *vk)
{
	kdo_initInstance(vk);
	kdo_initSurface(vk);
	kdo_initPhysicalDevice(vk);
	kdo_initDevice(vk);
	kdo_initSwapChain(vk);
	kdo_initViewsSwapChain(vk);
	kdo_initRenderPass(vk);
	kdo_initGraphicsPipeline(vk);
	kdo_initFramebuffers(vk);
	kdo_initSyncronisation(vk);
	kdo_initCore(vk);
}
