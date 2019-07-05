#include "DirectionalLight.h"



DirectionalLight::DirectionalLight()
{
	InitDefaultLight();
}

DirectionalLight::DirectionalLight(Vector4D * direction)
{
	m_lightDirection = direction->Normalise();
}


DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::InitDefaultLight()
{
	SetLightPosition(0.0, 20.0, 0.0);
	m_lightDirection = Vector4D(0, 0, 1);
	SetLightColour(1.0, 1.0, 1.0);
}

void DirectionalLight::SetLightDirection(Vector4D * LD)
{
	m_lightDirection = LD->Normalise();
}

Vector4D DirectionalLight::GetLightDirection()
{
	return m_lightDirection;
}
