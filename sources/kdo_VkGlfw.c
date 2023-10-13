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

static void kdo_windowResized(GLFWwindow* window, int width, int height)
{
	(void) width;
	(void) height;

	((Kdo_Vulkan *) glfwGetWindowUserPointer(window))->display.windowResized = 1;
}

void	kdo_initGlfw(Kdo_Vulkan *vk)
{
	char		**extensions;
	const char	**glfwExtensions;
	uint32_t	glfwExtensionsCount;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	vk->window.path = glfwCreateWindow(vk->info.windowWidth, vk->info.windowHeight, vk->info.applicationName, NULL, NULL);

	glfwSetWindowUserPointer(vk->window.path, vk);
	glfwSetFramebufferSizeCallback(vk->window.path, kdo_windowResized);
//	glfwSetInputMode (vk->window.path, GLFW_CURSOR , GLFW_CURSOR_DISABLED);

	glfwSetScrollCallback(vk->window.path, kdo_zoom);

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	KDO_VK_ALLOC(extensions, malloc((vk->info.instanceExtensionsCount + glfwExtensionsCount) * sizeof(char *)));

	memcpy(extensions, vk->info.instanceExtensions, vk->info.instanceExtensionsCount * sizeof(char *));
	memcpy(extensions + vk->info.instanceExtensionsCount, glfwExtensions, glfwExtensionsCount * sizeof(char *));
	vk->info.instanceExtensions			= (const char **) extensions;
	vk->info.instanceExtensionsCount	+= glfwExtensionsCount;
}
