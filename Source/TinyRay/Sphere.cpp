/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <math.h>
#include <random>
#include "Sphere.h"
Sphere::Sphere()
{
	//the default sphere is centred around the origin of the world coordinate
	//default radius is 2.0

	m_centre.SetZero();
	m_radius = 2.0;
	m_primtype = PRIMTYPE_Sphere;
}

Sphere::Sphere(double x, double y, double z, double r)
{
	m_centre.SetVector(x, y, z);
	m_radius = r;
	m_primtype = PRIMTYPE_Sphere;
}

Sphere::~Sphere()
{
}

RayHitResult Sphere::IntersectByRay(Ray& ray)
{
	RayHitResult result = Ray::s_defaultHitResult;

	double t = FARFAR_AWAY;
	double t2 = FARFAR_AWAY;
	Vector4D normal;
	Vector4D distance;
	Vector4D inverseRay;
	double dotProductRay;
	double dotProductDist;
	double dotProductRayDist;
	double rayDistanceSqrd;
	double squareRoot;
	Vector4D intersection_point;

	inverseRay.SetVector(-ray.GetRay()[0], -ray.GetRay()[1], -ray.GetRay()[2]);
	distance = ray.GetRayStart() - m_centre;

	dotProductDist = distance.DotProduct(distance);
	dotProductRay = ray.GetRay().DotProduct(ray.GetRay());
	dotProductRayDist = ray.GetRay().DotProduct(distance);

	rayDistanceSqrd = dotProductRayDist * dotProductRayDist;
	squareRoot = sqrtf((rayDistanceSqrd - (dotProductRay * (dotProductDist - (m_radius * m_radius)))));

	t = (inverseRay.DotProduct(distance) + squareRoot / dotProductRay);
	t2 = (inverseRay.DotProduct(distance) - squareRoot / dotProductRay);

	if (t2 < t)
		t = t2;

	//Calculate the exact location of the intersection using the result of t
	intersection_point = ray.GetRayStart() + ray.GetRay()*t;
	//normal.SetVector((rand() % 100 + 1) / 100, (rand() % 100 + 1) / 100, (rand() % 100 + 1) / 100);
	//Normals vary across the surface of a sphere
	normal = intersection_point - m_centre;
	normal.Normalise();

	if (t>0.0 && t < FARFAR_AWAY)
	{
		result.t = t;
		result.data = this;
		result.point = intersection_point;
		result.normal = normal;
	}

	return result;
}
