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

#ifndef KDO_VKSWAPCHAIN_H
# define KDO_VKSWAPCHAIN_H

# include "kdo_Vulkan.h"
# include "kdo_VkPhysicalDevice.h"
# include "kdo_VkRenderPass.h"
# include "kdo_VkFramebuffers.h"
# include "kdo_VkSyncronisation.h"
# include "kdo_VkDisplay.h"

void    kdo_initViewsSwapChain(Kdo_Vulkan *vk);
void	kdo_initSwapChain(Kdo_Vulkan *vk);
void    kdo_recreateSwapchain(Kdo_Vulkan *vk);

#endif
