#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform Push
{
	mat4	camera;
	vec3	cameraPos;
	vec3	center;
	float	radius;
	float	Im;
	float	aspect;
	uint	cubeSize;
	uint	iterMax;
} push;

layout(location = 0) out vec3	coord;

void main()
{ 
	uint	cubeSize	= push.cubeSize;
	vec3	worldPos	= vec3(gl_VertexIndex % cubeSize, gl_VertexIndex / cubeSize , gl_InstanceIndex) / cubeSize * 2 - 1;
	vec4	pos			= push.camera * vec4(worldPos, 1.0);

	gl_PointSize	= 2 / length(push.cameraPos - worldPos);
	gl_Position		= vec4(pos.x, pos.y * push.aspect, pos.z, pos.w);
	coord			= worldPos;
}
