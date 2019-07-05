/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
* 
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include "Triangle.h"

Triangle::Triangle()
{
	m_vertices[0] = Vector4D(-1.0, 0.0, -5.0);
	m_vertices[1] = Vector4D(0.0, 1.0, -5.0);
	m_vertices[2] = Vector4D(1.0, 0.0, -5.0);
	m_normal = Vector4D(0.0, 0.0, 1.0);
	m_primtype = PRIMTYPE_Triangle;
	normalsSet = false;
}

Triangle::Triangle(Vector4D pos1, Vector4D pos2, Vector4D pos3)
{
	SetTriangle(pos1, pos2, pos3);

	m_primtype = PRIMTYPE_Triangle;
	normalsSet = false;
}


Triangle::~Triangle()
{
}

void Triangle::SetTriangle(Vector4D v0, Vector4D v1, Vector4D v2)
{
	m_vertices[0] = v0;
	m_vertices[1] = v1;
	m_vertices[2] = v2;

	//Calculate Normal
	m_edge1 = m_vertices[1] - m_vertices[0];
	m_edge2 = m_vertices[2] - m_vertices[0];
	
	Vector4D Norm = m_edge1.CrossProduct(m_edge2);
	Norm.Normalise();
	m_normal = Norm;
}

void Triangle::SetNormals(Vector4D n1, Vector4D n2, Vector4D n3)
{
	m_vertexNormals[0] = n1;
	m_vertexNormals[1] = n2;
	m_vertexNormals[2] = n3;
	normalsSet = true;
}

void Triangle::SetTexCoords(Vector4D tc1, Vector4D tc2, Vector4D tc3)
{
	m_texCoords[0] = tc1;
	m_texCoords[1] = tc2;
	m_texCoords[2] = tc3;
}

void Triangle::ApplyMatrix(const Matrix4x4& trans)
{
	m_vertices[0] = trans * m_vertices[0];
	m_vertices[1] = trans * m_vertices[1];
	m_vertices[2] = trans * m_vertices[2];
	m_edge1 = m_vertices[1] - m_vertices[0];
	m_edge2 = m_vertices[2] - m_vertices[0];
}

void Triangle::ApplyScale(double scale)
{
	m_vertices[0] = m_vertices[0] * scale;
	m_vertices[1] = m_vertices[1] * scale;
	m_vertices[2] = m_vertices[2] * scale;
	m_edge1 = m_vertices[1] - m_vertices[0];
	m_edge2 = m_vertices[2] - m_vertices[0];
}

RayHitResult Triangle::IntersectByRay(Ray& ray)
{
	// TODO: Calculate the intersection between in the input ray and this triangle
	// If you have implemented ray-plane intersection, you have done half the work for ray-triangle intersection.
	// The remaining test is to check if the intersection point is inside the triangle
	//
	// Similar to the plane case, you should check if the ray is parallel to the triangle
	// and check if the ray intersects the triangle from the front or the back

	RayHitResult result = Ray::s_defaultHitResult;
	double t = FARFAR_AWAY;

	double e = 0.000001;
	double det;
	double inv_det = 0;
	double u;
	double v;

	//moller trumbore inside test
	Vector4D P = ray.GetRay().CrossProduct(m_edge2);
	Vector4D Q;
	Vector4D T;

	det = m_edge1.DotProduct(P);

	if (det > -e && det < e)
		return result;

	inv_det = 1.0 / det;

	T = ray.GetRayStart() - m_vertices[0];

	u = T.DotProduct(P) * inv_det;
	if (u < 0.0 || u > 1.0)
		return result;

	Q = T.CrossProduct(m_edge1);

	v = ray.GetRay().DotProduct(Q) * inv_det;
	if (v < 0.0 || u + v  > 1.0)
		return result;

	t = m_edge2.DotProduct(Q) * inv_det;

	if (t < FARFAR_AWAY) 
	{ //ray intersection

	  //Calculate the exact location of the intersection using the result of t
		Vector4D intersection_point = ray.GetRayStart() + ray.GetRay()*t;

		if (normalsSet)
		{
			//http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
			Vector4D bary;

			// The area of a triangle is 
			double areaABC = m_normal.DotProduct(m_edge1.CrossProduct(m_edge2));
			double areaPBC = m_normal.DotProduct((m_vertices[1] - intersection_point).CrossProduct(m_vertices[2] - intersection_point));
			double areaPCA = m_normal.DotProduct((m_vertices[2] - intersection_point).CrossProduct(m_vertices[0] - intersection_point));

			bary[0] = areaPBC / areaABC; // alpha
			bary[1] = areaPCA / areaABC; // beta
			bary[2] = 1.0f - bary[0] - bary[1]; // gamma

			m_normal = m_vertexNormals[0] * bary[0] + m_vertexNormals[1] * bary[1] + m_vertexNormals[2] * bary[2];

			result.UV = m_texCoords[0] * bary[0] + m_texCoords[1] * bary[1] + m_texCoords[2] * bary[2];
		}

		result.t = t;
		result.normal = m_normal;
		result.point = intersection_point;
		result.data = this;
		return result;
	}
	
	return result;
}
