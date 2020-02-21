#version 420

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(std140, binding = 2) uniform LightScene
{
	uniform vec3 uSceneAmbient;
};

layout(std140, binding = 3) uniform Light
{
	uniform vec3 uLightColor;
	uniform vec3 uLightPosition;
	uniform vec3 uLightDirection;
	uniform vec4 uLightAttenuation; 
};

#define uLightConstantAttenuation uLightAttenuation.x
#define uLightLinearAttenuation uLightAttenuation.y
#define uLightQuadAttenuation uLightAttenuation.z
#define uLightRadius uLightAttenuation.w

layout(std140, binding = 11) uniform ambLight
{
	uniform bool uAmbLightActive;
};
layout(std140, binding = 12) uniform speLight
{
	uniform bool uSpeLightActive;
};
layout(std140, binding = 13) uniform rimLight
{
	uniform bool uRimLightActive;
};
layout(std140, binding = 14) uniform diffLight
{
	uniform bool uDiffLightActive;
};
layout(std140, binding = 15) uniform diffRamp
{
	uniform bool uDiffRampActive;
};
layout(std140, binding = 16) uniform specRamp
{
	uniform bool uSpecRampActive;
};

layout(std140, binding = 5) uniform Toon
{
	uniform bool uToonActive;
};
layout(binding = 31) uniform sampler2D uTexToonRamp;
layout(binding = 32) uniform sampler2D uTexToonRamp2;

uniform float uMaterialSpecularExponent = 64.0;

layout(binding = 0) uniform sampler2D uTexAlbedo;
layout(binding = 1) uniform sampler2D uTexEmissive;
layout(binding = 2) uniform sampler2D uTexSpecular;

in vec2 texcoord;
in vec3 norm;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec2 texOffset = texcoord;
	vec4 objectColor = texture(uTexAlbedo, texOffset);

	if(uAmbLightActive)
	{
		float ambientStrength = 0.5;
		vec3 ambient = ambientStrength * uLightColor;
		vec3 result = ambient * objectColor.rgb;
		outColor = vec4(result, objectColor.a);
	}
	else
	{
		float ambientStrength = 0.0;
		vec3 ambient = ambientStrength * uLightColor;
		vec3 result = ambient * objectColor.rgb;
		outColor = vec4(result, objectColor.a);
	}
	if(uSpeLightActive)
	{
		vec3 normal = normalize(norm);
		vec3 lightVec = uLightPosition - pos;
		float dist = length(lightVec);
		vec3 lightDir = lightVec / dist;
		vec3 reflection = reflect(-lightDir, normal);
		vec3 eye = normalize(-pos);
		float specularStrength = 0.5;
		float viewToReflect = dot(eye,reflection);
		if(uSpecRampActive)
		{
			outColor.rgb += uLightColor * objectColor.rgb * 
				texture(uTexToonRamp,vec2(viewToReflect, 0.5)).rgb;
		}
		else
		{
			float spec = pow(max(viewToReflect,0.0),16);
			vec3 specular = specularStrength * spec * uLightColor;
			outColor.rgb += specular * objectColor.rgb;
		}
	}
	if(uRimLightActive)
	{	
		vec3 eye = normalize(-pos);
		float vdn = 1.0 - max( dot( eye, norm),0.0 );
		outColor.rgb += vdn;
	}
	if(uDiffLightActive)
	{
		vec3 normal = normalize(norm);
		vec3 lightVec = uLightPosition - pos;
		float dist = length(lightVec);
		vec3 lightDir = lightVec / dist;
		float NdotL = dot(normal, lightDir);
		if(uDiffRampActive)
		{
			NdotL = NdotL * 0.5 + 0.5;
			outColor.rgb += objectColor.rgb * uLightColor 
				* texture(uTexToonRamp2, vec2(NdotL, 0.5)).rgb;
		}
		else
		{
			float diff = max(NdotL, 0.0);
			vec3 diffuse = diff * uLightColor;
			vec3 result = diffuse * objectColor.rgb;
			outColor.rgb += result;
		}
	}
	outColor.rgb += texture(uTexEmissive, texOffset).rgb;
}
