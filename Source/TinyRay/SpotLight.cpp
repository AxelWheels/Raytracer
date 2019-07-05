#include "SpotLight.h"
#include <math.h>

#define PI 3.14159265359
#define RAD_TO_DEG(x) x * 180.0 / PI
SpotLight::SpotLight()
{
	InitDefaultLight();
}


SpotLight::~SpotLight()
{
}

void SpotLight::InitDefaultLight()
{
	SetLightPosition(0.0, 20.0, 0.0);
	m_lightDirection = Vector4D(0, -1, -1);
	SetLightColour(1.0, 1.0, 1.0);
	m_umbraAngle = 0.9;
	m_PenumbraAngle = 0.7;
}

void SpotLight::SetLightDirection(Vector4D * LD)
{
	m_lightDirection = LD->Normalise();
}

double SpotLight::AngularAttenuation(double theta)
{
	double angularAttenuation = 0.0;
	double tAngle = RAD_TO_DEG(acos(theta));
	double uAngle = RAD_TO_DEG(acos(m_umbraAngle));
	double pAngle = RAD_TO_DEG(acos(m_PenumbraAngle));

	if (tAngle > uAngle)
		return 0.0;
	else if (0.0 <= tAngle && tAngle <= pAngle)
		return 1.0;
	else if (pAngle < tAngle && tAngle<= uAngle)
		angularAttenuation = pow((theta - cos(m_umbraAngle)) / (cos(m_PenumbraAngle) - cos(m_umbraAngle)), 8);

	return angularAttenuation;
}
