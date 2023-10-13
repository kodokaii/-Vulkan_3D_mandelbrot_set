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

#include "kdo_VkRenderPass.h"

void	kdo_initRenderPass(Kdo_Vulkan *vk)
{
	VkFormat				depthFormats[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	VkRenderPassCreateInfo	renderPassInfo;
	VkAttachmentDescription	colorAttachment;
	VkAttachmentDescription depthAttachment;
	VkAttachmentReference	depthAttachmentRef;
	VkAttachmentReference	colorAttachmentRef;
	VkSubpassDescription	subpass;
	VkSubpassDependency		dependency;

	vk->framebuffer.depth.format = kdo_findFormat(vk, depthFormats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	colorAttachment.flags			= 0;
	colorAttachment.format			= vk->swapChain.imagesFormat.format;
	colorAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colorAttachmentRef.attachment	= 0;
	colorAttachmentRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	depthAttachment.flags			= 0;
	depthAttachment.format			= vk->framebuffer.depth.format;
	depthAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachmentRef.attachment	= 1;
	depthAttachmentRef.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpass.flags					= 0;
	subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount	= 0;
	subpass.pInputAttachments		= NULL;
	subpass.colorAttachmentCount	= 1;
	subpass.pColorAttachments		= &colorAttachmentRef;
	subpass.pResolveAttachments		= NULL;
	subpass.pDepthStencilAttachment	= &depthAttachmentRef;
	subpass.preserveAttachmentCount	= 0;
	subpass.pPreserveAttachments	= NULL;

	dependency.srcSubpass			= VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass			= 0;
	dependency.srcStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;;
	dependency.srcAccessMask		= 0;
	dependency.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags		= 0;

	VkAttachmentDescription	attachments[2]	= {colorAttachment, depthAttachment};

	renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext			= NULL;
	renderPassInfo.flags			= 0;
	renderPassInfo.attachmentCount	= 2;
	renderPassInfo.pAttachments		= attachments;
	renderPassInfo.subpassCount		= 1;
	renderPassInfo.pSubpasses		= &subpass;
	renderPassInfo.dependencyCount	= 1;
	renderPassInfo.pDependencies	= &dependency;
	if (vkCreateRenderPass(vk->device.path, &renderPassInfo, NULL, &vk->renderPass.path) != VK_SUCCESS)
		kdo_cleanup(vk, "Render pass creation failed", 10);
}
