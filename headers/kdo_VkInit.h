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

#ifndef KDO_VKINIT_H
# define KDO_VKINIT_H

# include "kdo_Vulkan.h"
# include "kdo_VkInstance.h"
# include "kdo_VkSurface.h"
# include "kdo_VkPhysicalDevice.h"
# include "kdo_VkDevice.h"
# include "kdo_VkSwapChain.h"
# include "kdo_VkRenderPass.h"
# include "kdo_VkGraphicsPipeline.h"
# include "kdo_VkFramebuffers.h"
# include "kdo_VkSyncronisation.h"
# include "kdo_VkCore.h"

void    kdo_initVulkan(Kdo_Vulkan *vk);

#endif
