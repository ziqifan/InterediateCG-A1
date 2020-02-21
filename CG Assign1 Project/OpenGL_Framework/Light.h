#pragma once
#include "Transform.h"
#include "UniformBuffer.h"

class Light : public Transform
{
public:
	enum LightType
	{
		Directional,
		Point,
		Spotlight
	};
	
	vec4 color;
	vec4 position;
	vec4 direction;
	
	float constantAtten;
	float linearAtten;
	float quadAtten;
	float radius;

	LightType type;

	Light();
	void init();

	void update(float dt);
	float calculateRadius();

	UniformBuffer _UBO;
};