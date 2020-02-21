#include "Light.h"

Light::Light()
{
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	position = vec4(0.0f);
	constantAtten = 1.0f;
	linearAtten = 0.1f;
	quadAtten = 0.001f;
}

void Light::init()
{
	_UBO.allocateMemory(sizeof(vec4) * 3 + sizeof(float) * 4);
}

void Light::update(float dt)
{
	Transform::update(dt);
	calculateRadius();
	_UBO.sendData(&color, sizeof(vec4) * 3 + sizeof(float) * 4, 0);
}

float Light::calculateRadius()
{
	float luminance = Dot((vec3(color)/color.w), vec3(0.3f, 0.59f, 0.11f));
	float luminanceMin = 0.05f;
	radius = (-linearAtten + sqrtf(linearAtten * linearAtten - 4.0f * quadAtten * (constantAtten - luminance / luminanceMin))) / (2.0f * quadAtten);
	return radius;
}
