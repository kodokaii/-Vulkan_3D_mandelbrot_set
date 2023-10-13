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

#include "kdo_VkGraphicsPipeline.h"

static void	kdo_readSpirV(Kdo_Vulkan *vk, const char *fileName, Kdo_Shader *shader)
{
	FILE	*file;

	if (!(file = fopen(fileName, "rb")))
	{
		printf("Can't open %s file\n", fileName);
		kdo_cleanup(vk, "File error", 9);
	}
	fseek(file, 0, SEEK_END);
	shader->codeSize = ftell(file);
	rewind(file);

	KDO_VK_ALLOC(shader->code, malloc(shader->codeSize * sizeof(char)));

	if (!(fread(shader->code, shader->codeSize, 1, file)))
		kdo_cleanup(vk, "Error while reading the shader", 11);

	fclose(file);
}

static void kdo_createShaderModule(Kdo_Vulkan *vk, Kdo_Shader *shader)
{
	VkShaderModuleCreateInfo	moduleInfo;

	moduleInfo.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleInfo.pNext		= NULL;
	moduleInfo.flags		= 0;
	moduleInfo.codeSize		= shader->codeSize;
	moduleInfo.pCode		= (uint32_t*) shader->code;
	if (vkCreateShaderModule(vk->device.path, &moduleInfo, NULL, &shader->module) != VK_SUCCESS)
		kdo_cleanup(vk, "Create shader module failed", 13);
}

static void	kdo_initPipelineLayout(Kdo_Vulkan *vk)
{
	VkPushConstantRange							constantsRange;
	VkPipelineLayoutCreateInfo					pipelineLayoutInfo;

	constantsRange.stageFlags	= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	constantsRange.offset		= 0;
	constantsRange.size			= sizeof(Kdo_VkPush);

	pipelineLayoutInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext						= NULL;
	pipelineLayoutInfo.flags						= 0;
	pipelineLayoutInfo.setLayoutCount				= 0;
	pipelineLayoutInfo.pSetLayouts					= NULL;
	pipelineLayoutInfo.pushConstantRangeCount		= 1;
	pipelineLayoutInfo.pPushConstantRanges			= &constantsRange;
	if (vkCreatePipelineLayout(vk->device.path, &pipelineLayoutInfo, NULL, &vk->graphicsPipeline.layout) != VK_SUCCESS)
		kdo_cleanup(vk, "Pipeline layout creation failed", 15);
}

void kdo_initGraphicsPipeline(Kdo_Vulkan *vk)
{
	VkGraphicsPipelineCreateInfo				graphicsPipelineInfo;	
	VkPipelineShaderStageCreateInfo				shaderStageInfo[2];
	VkPipelineVertexInputStateCreateInfo		vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo		inputAssemblyInfo;
	VkDynamicState								dynamicStates[]	= {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo			dynamicStateInfo;
	VkPipelineViewportStateCreateInfo			viewportStateInfo;
	VkPipelineRasterizationStateCreateInfo		rasterizerInfo;
	VkPipelineMultisampleStateCreateInfo		multisamplingInfo;
	VkPipelineDepthStencilStateCreateInfo		depthStencilInfo;
	VkPipelineColorBlendStateCreateInfo			colorBlendingInfo;
	VkPipelineColorBlendAttachmentState			colorBlendAttachment;

	kdo_initPipelineLayout(vk);

	kdo_readSpirV(vk, "shaders/vertex.spv", &vk->graphicsPipeline.vertexShader);	
	kdo_readSpirV(vk, "shaders/fragment.spv", &vk->graphicsPipeline.fragmentShader);	

	kdo_createShaderModule(vk, &vk->graphicsPipeline.vertexShader);
	kdo_createShaderModule(vk, &vk->graphicsPipeline.fragmentShader);

	shaderStageInfo[0].sType				= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo[0].pNext				= NULL;
	shaderStageInfo[0].flags				= 0;
	shaderStageInfo[0].stage				= VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageInfo[0].module				= vk->graphicsPipeline.vertexShader.module;
	shaderStageInfo[0].pName				= "main";
	shaderStageInfo[0].pSpecializationInfo	= NULL;

	shaderStageInfo[1].sType				= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo[1].pNext				= NULL;
	shaderStageInfo[1].flags				= 0;
	shaderStageInfo[1].stage				= VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageInfo[1].module				= vk->graphicsPipeline.fragmentShader.module;
	shaderStageInfo[1].pName				= "main";
	shaderStageInfo[1].pSpecializationInfo	= NULL;


	vertexInputInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext								= NULL;
	vertexInputInfo.flags								= 0;
	vertexInputInfo.vertexBindingDescriptionCount		= 0;
	vertexInputInfo.pVertexBindingDescriptions			= NULL;
	vertexInputInfo.vertexAttributeDescriptionCount		= 0;
	vertexInputInfo.pVertexAttributeDescriptions		= NULL;

	inputAssemblyInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.pNext						= NULL;
	inputAssemblyInfo.flags						= 0;
	inputAssemblyInfo.topology					= VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	inputAssemblyInfo.primitiveRestartEnable	= VK_FALSE;


	dynamicStateInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pNext				= NULL;
	dynamicStateInfo.flags				= 0;
	dynamicStateInfo.dynamicStateCount	= 2;
	dynamicStateInfo.pDynamicStates		= dynamicStates;

	viewportStateInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.pNext				= NULL;
	viewportStateInfo.flags				= 0;
	viewportStateInfo.viewportCount		= 1;
	viewportStateInfo.pViewports		= VK_NULL_HANDLE;
	viewportStateInfo.scissorCount		= 1;
	viewportStateInfo.pScissors			= VK_NULL_HANDLE;


	rasterizerInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.pNext						= NULL;
	rasterizerInfo.flags						= 0;
	rasterizerInfo.depthClampEnable				= VK_FALSE;
	rasterizerInfo.rasterizerDiscardEnable		= VK_FALSE;
	rasterizerInfo.polygonMode					= VK_POLYGON_MODE_FILL;
	rasterizerInfo.cullMode						= VK_CULL_MODE_BACK_BIT; 
	rasterizerInfo.frontFace					= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerInfo.depthBiasEnable				= VK_FALSE;
	rasterizerInfo.depthBiasConstantFactor		= 0.0;
	rasterizerInfo.depthBiasClamp				= 0.0;
	rasterizerInfo.depthBiasSlopeFactor			= 0.0;
	rasterizerInfo.lineWidth					= 1.0;

	multisamplingInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingInfo.pNext						= NULL;
	multisamplingInfo.flags						= 0;
	multisamplingInfo.rasterizationSamples		= VK_SAMPLE_COUNT_1_BIT;
	multisamplingInfo.sampleShadingEnable		= VK_FALSE;
	multisamplingInfo.minSampleShading			= 1.0;
	multisamplingInfo.pSampleMask				= NULL;
	multisamplingInfo.alphaToCoverageEnable		= VK_FALSE;
	multisamplingInfo.alphaToOneEnable			= VK_FALSE;

	depthStencilInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.pNext					= NULL;
	depthStencilInfo.flags					= 0;
	depthStencilInfo.depthTestEnable		= VK_TRUE;
	depthStencilInfo.depthWriteEnable		= VK_TRUE;
	depthStencilInfo.depthCompareOp			= VK_COMPARE_OP_LESS;
	depthStencilInfo.depthBoundsTestEnable	= VK_FALSE;
	depthStencilInfo.stencilTestEnable		= VK_FALSE;
	depthStencilInfo.front.failOp			= 0;
	depthStencilInfo.front.passOp			= 0;
	depthStencilInfo.front.depthFailOp		= 0;
	depthStencilInfo.front.compareOp		= 0;
	depthStencilInfo.front.compareMask		= 0;
	depthStencilInfo.front.writeMask		= 0;
	depthStencilInfo.front.reference		= 0;
	depthStencilInfo.back.failOp			= 0;
	depthStencilInfo.back.passOp			= 0;
	depthStencilInfo.back.depthFailOp		= 0;
	depthStencilInfo.back.compareOp			= 0;
	depthStencilInfo.back.compareMask		= 0;
	depthStencilInfo.back.writeMask			= 0;
	depthStencilInfo.back.reference			= 0;
	depthStencilInfo.minDepthBounds			= 0.0f;
	depthStencilInfo.maxDepthBounds			= 1.0f;

	colorBlendAttachment.blendEnable			= VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD;
	colorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorBlendingInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingInfo.pNext					= NULL;
	colorBlendingInfo.flags					= 0;
	colorBlendingInfo.logicOpEnable			= VK_FALSE;
	colorBlendingInfo.logicOp				= VK_LOGIC_OP_COPY;
	colorBlendingInfo.attachmentCount		= 1;
    colorBlendingInfo.pAttachments			= &colorBlendAttachment;
	colorBlendingInfo.blendConstants[0]		= 0.0;
	colorBlendingInfo.blendConstants[1]		= 0.0;
	colorBlendingInfo.blendConstants[2]		= 0.0;
	colorBlendingInfo.blendConstants[3]		= 0.0;

	graphicsPipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineInfo.pNext					= NULL;
	graphicsPipelineInfo.flags					= 0;
	graphicsPipelineInfo.stageCount				= 2;
	graphicsPipelineInfo.pStages				= shaderStageInfo;
	graphicsPipelineInfo.pVertexInputState		= &vertexInputInfo;
	graphicsPipelineInfo.pInputAssemblyState	= &inputAssemblyInfo;
	graphicsPipelineInfo.pTessellationState		= NULL;
	graphicsPipelineInfo.pViewportState			= &viewportStateInfo;
	graphicsPipelineInfo.pRasterizationState	= &rasterizerInfo;
	graphicsPipelineInfo.pMultisampleState		= &multisamplingInfo;
	graphicsPipelineInfo.pDepthStencilState		= &depthStencilInfo;
	graphicsPipelineInfo.pColorBlendState		= &colorBlendingInfo;
	graphicsPipelineInfo.pDynamicState			= &dynamicStateInfo;
	graphicsPipelineInfo.layout					= vk->graphicsPipeline.layout;
	graphicsPipelineInfo.renderPass				= vk->renderPass.path;
	graphicsPipelineInfo.subpass				= 0;
	graphicsPipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;
	graphicsPipelineInfo.basePipelineIndex		= -1;
	if (vkCreateGraphicsPipelines(vk->device.path, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, NULL, &vk->graphicsPipeline.path) != VK_SUCCESS)
			kdo_cleanup(vk, "Graphics pipeline creation failed", 16);

	KDO_FREE(vk->graphicsPipeline.vertexShader.code)
	KDO_FREE(vk->graphicsPipeline.fragmentShader.code)
	KDO_DESTROY(vkDestroyShaderModule, vk->device.path, vk->graphicsPipeline.vertexShader.module)
	KDO_DESTROY(vkDestroyShaderModule, vk->device.path, vk->graphicsPipeline.fragmentShader.module)
}
