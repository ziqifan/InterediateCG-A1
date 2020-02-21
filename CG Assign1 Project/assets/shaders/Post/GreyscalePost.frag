/*
===========================================================================

SpeedStorm Shader Source Code
Copyright (C) 2015-2017 Stephen Thompson

===========================================================================
*/

#version 420 //Version of OpenGL we're using. - 4.2

uniform sampler2D uSceneTex; 

layout(std140, binding = 17) uniform gradWarm
{
	uniform bool uGradWarmActive;
}; 
layout(std140, binding = 18) uniform gradCool
{
	uniform bool uGradCoolActive;
};
layout(std140, binding = 19) uniform gradCustom
{
	uniform bool uGradCustomActive;
};

layout(binding = 24) uniform sampler3D warmTexure;
layout(binding = 25) uniform sampler3D coolTexure;
layout(binding = 26) uniform sampler3D customTexure;

uniform float lutSize = 64;

in vec2 texcoord;
out vec4 outColor;

void main()
{
	vec4 source = texture(uSceneTex, texcoord);
	if(uGradWarmActive)
	{
		vec3 scale = vec3((lutSize - 1.0) / lutSize);
		vec3 offset = vec3(1.0/2.0*lutSize);
		source.rgb = texture(warmTexure, source.rgb * scale + offset).rgb;
	}

	if(uGradCoolActive)
	{
		vec3 scale = vec3((lutSize - 1.0) / lutSize);
		vec3 offset = vec3(1.0/2.0*lutSize);
		source.rgb = texture(coolTexure, source.rgb * scale + offset).rgb;
	}

	if(uGradCustomActive)
	{
		vec3 scale = vec3((lutSize - 1.0) / lutSize);
		vec3 offset = vec3(1.0/2.0*lutSize);
		source.rgb = texture(customTexure, source.rgb * scale + offset).rgb;
	}

	outColor = source;

}