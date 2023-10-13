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

#include "kdo_VkCompute.h"

static void	kdo_reset(Kdo_Vulkan *vk)
{
	vk->compute.push.center[0]  = vk->info.startCenter[0];
	vk->compute.push.center[1]  = vk->info.startCenter[1];
	vk->compute.push.center[2]  = vk->info.startCenter[2];
	vk->compute.push.Im			= 0;
	vk->compute.push.radius     = vk->info.startRadius;
	vk->compute.push.cubeSize	= vk->info.startCubeSize;
	vk->compute.push.iterMax    = vk->info.startIterMax;
}

static void	kdo_updateCamera(Kdo_Vulkan *vk)
{
	float	cameraSpeed			= 0.05f;
	float	cameraSensitivity	= glm_rad(0.15f);
	float	ImSensitivity		= vk->compute.push.radius * vk->compute.push.radius * 0.001;
	float	cubeSensitivity		= glm_max(vk->compute.push.cubeSize / 100, 1);
	float	iterSensitivity		= glm_max(vk->compute.push.iterMax / 10, 1);
	double	yMousse;
	double	xMousse;
	mat4	view;
	mat4	proj;
	vec3	watch;
	vec3	right;
	vec3	front;
	vec3	top = {0.0f, 0.0f, -1.0f};

	if ((glfwGetKey(vk->window.path, GLFW_KEY_ENTER)) == GLFW_PRESS)
		kdo_reset(vk);

	glfwGetCursorPos(vk->window.path, &xMousse, &yMousse);
	vk->compute.camera.yaw		= vk->compute.camera.yaw + ((xMousse - vk->compute.camera.xMouse) * cameraSensitivity);
	vk->compute.camera.pitch	= glm_clamp(vk->compute.camera.pitch + ((yMousse - vk->compute.camera.yMouse) * cameraSensitivity), glm_rad(-89.99), glm_rad(89.99));
	watch[0] = cos(vk->compute.camera.pitch) * cos(vk->compute.camera.yaw);
	watch[1] = cos(vk->compute.camera.pitch) * sin(vk->compute.camera.yaw);
	watch[2] = sin(vk->compute.camera.pitch);

	glm_vec3_crossn(watch, top, right);
	glm_vec3_crossn(top, right, front);

	glm_vec3_scale(top, cameraSpeed, top);
	glm_vec3_scale(right, cameraSpeed, right);
	glm_vec3_scale(front, cameraSpeed, front);

	if ((glfwGetKey(vk->window.path, GLFW_KEY_W)) == GLFW_PRESS)
		glm_vec3_add(vk->compute.push.cameraPos, front, vk->compute.push.cameraPos);

	if ((glfwGetKey(vk->window.path, GLFW_KEY_S)) == GLFW_PRESS)
		glm_vec3_sub(vk->compute.push.cameraPos, front, vk->compute.push.cameraPos);

	if ((glfwGetKey(vk->window.path, GLFW_KEY_D)) == GLFW_PRESS)
		glm_vec3_add(vk->compute.push.cameraPos, right, vk->compute.push.cameraPos);

	if ((glfwGetKey(vk->window.path, GLFW_KEY_A)) == GLFW_PRESS)
		glm_vec3_sub(vk->compute.push.cameraPos, right, vk->compute.push.cameraPos);

	if ((glfwGetKey(vk->window.path, GLFW_KEY_SPACE)) == GLFW_PRESS)
		glm_vec3_add(vk->compute.push.cameraPos, top, vk->compute.push.cameraPos);

	if ((glfwGetKey(vk->window.path, GLFW_KEY_LEFT_CONTROL)) == GLFW_PRESS)
		glm_vec3_sub(vk->compute.push.cameraPos, top, vk->compute.push.cameraPos);
	
	glm_look(vk->compute.push.cameraPos, watch, GLM_ZUP, view);
	glm_perspective(glm_rad(60.0f), vk->swapChain.imagesExtent.width / vk->swapChain.imagesExtent.height, 0.1f, 50.0f, proj);

	glm_mat4_mul(proj, view, vk->compute.push.camera);

	vk->compute.camera.xMouse = xMousse;
	vk->compute.camera.yMouse = yMousse;

	vk->compute.push.aspect	= (float)vk->swapChain.imagesExtent.width / (float)vk->swapChain.imagesExtent.height;

	if ((glfwGetKey(vk->window.path, GLFW_KEY_Q)) == GLFW_PRESS)
		vk->compute.push.Im	+=	ImSensitivity;

	if ((glfwGetKey(vk->window.path, GLFW_KEY_E)) == GLFW_PRESS)
		vk->compute.push.Im	-=	ImSensitivity;

	if ((glfwGetKey(vk->window.path, GLFW_KEY_RIGHT)) == GLFW_PRESS)
		vk->compute.push.iterMax	+=	iterSensitivity;

	if ((glfwGetKey(vk->window.path, GLFW_KEY_LEFT)) == GLFW_PRESS && vk->compute.push.iterMax)
		vk->compute.push.iterMax	-=	iterSensitivity;

	if ((glfwGetKey(vk->window.path, GLFW_KEY_UP)) == GLFW_PRESS)
		vk->compute.push.cubeSize	+=	cubeSensitivity;

	if ((glfwGetKey(vk->window.path, GLFW_KEY_DOWN)) == GLFW_PRESS && vk->compute.push.cubeSize)
		vk->compute.push.cubeSize	-=	cubeSensitivity;
}

void	kdo_zoom(GLFWwindow* window, double xoffset, double yoffset)
{
	Kdo_Vulkan	*vk;
	double		scrollSensitivity;

	(void)	xoffset;
	vk	= ((Kdo_Vulkan *) glfwGetWindowUserPointer(window));

	scrollSensitivity	=	vk->compute.push.radius * 0.1;
	vk->compute.push.radius	-= scrollSensitivity * yoffset;
}

void kdo_compute(Kdo_Vulkan *vk)
{
	vk->compute.currentTime	= glfwGetTime();


	if (vk->compute.startTime == 0)
	{
		kdo_reset(vk);
		vk->compute.startTime = vk->compute.currentTime;
	}

	vk->compute.fps++;
	if (1 < vk->compute.currentTime - vk->compute.secondTime)
	{
		printf("%d\n", vk->compute.fps);
		vk->compute.fps = 0;
		vk->compute.secondTime = vk->compute.currentTime;
	}

	kdo_updateCamera(vk);
}
