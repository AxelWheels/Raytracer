#pragma once
#include "DirectionalLight.h"
class SpotLight : public DirectionalLight
{
private:
	double m_umbraAngle;
	double m_PenumbraAngle;
public:
	SpotLight();
	~SpotLight();

	void InitDefaultLight() override;
	void SetLightDirection(Vector4D * LD) override;
	double AngularAttenuation(double theta);
};

