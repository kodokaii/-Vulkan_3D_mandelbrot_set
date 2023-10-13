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

#ifndef KDO_VULKAN_H
# define KDO_VULKAN_H

# define CGLM_FORCE_LEFT_HANDED
# define CGLM_FORCE_DEPTH_ZERO_TO_ONE
# include "cglm/cglm.h"

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "kdo_VkBuffer.h"

# define SZ_SHADER				2

# define QUEUES_COUNT			3
# define PRESENT_QUEUE			0
# define GRAPHIC_QUEUE			1
# define TRANSFER_QUEUE			2

# define KDO_VK_ALLOC(ptr, alloc)	if (!(ptr = alloc)) kdo_cleanup(vk, "CPU Memory allocation error", 12)

typedef struct Kdo_VkPush
{
	mat4		camera;
	vec3		cameraPos;
	int			pad;
	vec3		center;
	float		radius;
	float		Im;
	float		aspect;
	uint32_t	cubeSize;
	uint32_t	iterMax;
}	Kdo_VkPush;

typedef struct Kdo_Shader
{
	char			*code;
	long			codeSize;
	VkShaderModule  module;
}	Kdo_Shader;

typedef struct Kdo_VkDepthBuffer
{
	VkImage			image;
	VkDeviceMemory	memory;
	VkImageView		view;
	VkFormat		format;
}	Kdo_VkDepthBuffer;

typedef struct Kdo_VkQueue
{
	VkQueue		path;
	uint32_t	index;
	uint32_t	familyIndex;
}	Kdo_VkQueue;

typedef struct Kdo_SynImage
{
	struct Kdo_SynImage	**path;
	VkFence				*renderFinishedFence;
}	Kdo_SynImage;

typedef struct Kdo_VkQueueInfo
{	uint32_t		count;
	float			priority;
	VkQueueFlags	requiredFlags;
	VkQueueFlags	noRequiredFlags;
	VkQueueFlags	preferredFlags;
	VkQueueFlags	noPreferredFlags;
	VkBool32		presentSupport;
}	Kdo_VkQueueInfo;

typedef	struct Kdo_VkInitInfo
{
	const char					*applicationName;
	const char					*engineName;
	uint32_t					instanceExtensionsCount;
	const char					**instanceExtensions;
	uint32_t					validationLayersCount;
	const char					**validationLayers;
	uint32_t					deviceExtensionsCount;
	const char					**deviceExtensions;
	int							windowWidth;
	int							windowHeight;
	Kdo_VkQueueInfo				queuesInfo[QUEUES_COUNT];
	VkPresentModeKHR			presentMode;
	vec3						startCenter;
	float						startRadius;
	uint32_t					startCubeSize;
	uint32_t					startIterMax;
}	Kdo_VkInitInfo;

typedef struct Kdo_VkWindow
{
	GLFWwindow	*path;
}	Kdo_VkWindow;

typedef struct Kdo_VkInstance
{
	VkInstance	path;
}	Kdo_VkInstance;

typedef struct Kdo_VkSurface
{
	VkSurfaceKHR	path;
}	Kdo_VkSurface;

typedef struct Kdo_VkSwapChainProperties
{
	VkSurfaceCapabilitiesKHR	capabilities;
	uint32_t					formatsCount;
    VkSurfaceFormatKHR			*formats;
	uint32_t					presentModesCount;
    VkPresentModeKHR			*presentModes;
}	Kdo_VkSwapChainProperties;

typedef struct Kdo_VkQueueProperties
{
	VkQueueFamilyProperties	*path;
	VkBool32				*presentSupport;
	uint32_t				famillesCount;
}	Kdo_VkQueueProperties;

typedef struct Kdo_VkPhysicalDevice
{
	VkPhysicalDevice					path;
	VkPhysicalDeviceProperties			properties;
	VkPhysicalDeviceFeatures			features;
	Kdo_VkSwapChainProperties			swapChainProperties;
	Kdo_VkQueueProperties				queueProperties;
	VkPhysicalDeviceMemoryProperties	memoryProperties;
}	Kdo_VkPhysicalDevice;

typedef struct Kdo_VkDevice
{
	VkDevice	path;
	Kdo_VkQueue	queues[QUEUES_COUNT];
}	Kdo_VkDevice;

typedef struct Kdo_VkSwapChain
{
	VkSwapchainKHR		path;
	uint32_t			imagesCount;
	VkImage				*images;
	VkImageView			*views;
	VkExtent2D			imagesExtent;
	VkSurfaceFormatKHR	imagesFormat;
}	Kdo_VkSwapChain;

typedef struct Kdo_VkRenderPass
{
	VkRenderPass	path;
}	Kdo_VkRenderPass;

typedef struct Kdo_VkGraphicsPipeline
{
	VkPipeline				path;
	Kdo_Shader				vertexShader;
	Kdo_Shader				fragmentShader;
	VkPipelineLayout		layout;
}	Kdo_VkGraphicsPipeline;

typedef struct Kdo_VkFramebuffer
{
	VkFramebuffer		*path;
	Kdo_VkDepthBuffer	depth;
}	Kdo_VkFramebuffer;

typedef struct Kdo_VkSemaphore
{
	VkSemaphore	*imageAvailable;
	VkSemaphore *renderFinished;
}	Kdo_VkSemaphore;

typedef struct Kdo_VkFence
{
	VkFence	*renderFinished;
}	Kdo_VkFence;

typedef struct Kdo_VkSampler
{
	VkSampler	basic;
}	Kdo_VkSampler;

typedef struct Kdo_VkCore
{
	VkCommandPool		transferPool;
}	Kdo_VkCore;

typedef struct Kdo_VkRenderPool
{
	VkCommandPool	path;
	VkCommandBuffer	main;
}	Kdo_VkRenderPool;

typedef struct Kdo_VkDisplay
{
	Kdo_VkRenderPool	*renderPool;
	uint32_t			currentImage;
	uint32_t			currentFrame;
	uint32_t			maxParallelFrame;
	uint32_t			windowResized;
	Kdo_SynImage		*frameToImage;
	Kdo_SynImage		**imageToFrame;
}	Kdo_VkDisplay;

typedef struct Kdo_VkCamera
{
	double	moveTime;
	double  xMouse;
	double	yMouse;
	float	yaw;
	float	pitch;
} Kdo_VkCamera;

typedef struct Kdo_VkCompute
{
	Kdo_VkCamera	camera;
	Kdo_VkPush		push;
	int				fps;
	double			startTime;
	double			currentTime;
	double			secondTime;
}	Kdo_VkCompute;

typedef struct Kdo_Vulkan
{
	Kdo_VkInitInfo			info;
	Kdo_VkWindow			window;
	Kdo_VkInstance			instance;
	Kdo_VkSurface			surface;
	Kdo_VkPhysicalDevice	physicalDevice;
	Kdo_VkDevice			device;
	Kdo_VkSwapChain			swapChain;
	Kdo_VkRenderPass		renderPass;	
	Kdo_VkGraphicsPipeline	graphicsPipeline;
	Kdo_VkFramebuffer		framebuffer;
	Kdo_VkSemaphore			semaphore;
	Kdo_VkFence				fence;
	Kdo_VkCore				core;
	Kdo_VkDisplay			display;
	Kdo_VkCompute			compute;
}	Kdo_Vulkan;

# include "kdo_VkCleanup.h"
# include "kdo_VkFunction.h"

#endif
