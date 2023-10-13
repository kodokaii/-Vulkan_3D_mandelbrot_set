#version 450
#extension GL_ARB_separate_shader_objects : enable

#define	SQUARE(X)	X * X

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

layout(location = 0) in vec3	coord;

layout(location = 0) out vec4	outColor;

void main()
{
	vec3	C	= vec3(push.center) + coord * push.radius;
	vec2	Zn	= vec2(C.z, push.Im);  
	vec2	Zn_1;
	vec2	Zn2	= SQUARE(Zn);

	uint	iterMax	= push.iterMax;
	uint	i		= 0;

	float	len		= 0;

	while (i <= iterMax && Zn2.x + Zn2.y < 4)
	{
		Zn_1	= vec2(Zn2.x - Zn2.y + C.x, 2 * Zn.x * Zn.y + C.y);
		len		+= length(Zn_1 - Zn);
		Zn		= Zn_1;
		Zn2		= SQUARE(Zn);
		i++;
	}
	if (i <= iterMax)
		discard;

	vec3	color	= vec3(0.1, 0.1, 1) * len / (iterMax * 4) + vec3(0.1, 1, 0.1) * abs(coord.z);

	outColor	= vec4(color, 1);
}
