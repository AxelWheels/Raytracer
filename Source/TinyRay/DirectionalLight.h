#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
protected:
	Vector4D m_lightDirection;
public:
	DirectionalLight();
	DirectionalLight(Vector4D *direction);
	virtual ~DirectionalLight();

	virtual void InitDefaultLight() override;
	virtual void SetLightDirection(Vector4D * LD);
	virtual Vector4D GetLightDirection();
};

