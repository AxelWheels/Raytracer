/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#pragma once
#include "Primitive.h"
#include "Vector4D.h"
#include "Matrix4x4.h"
#include "Ray.h"
#include <vector>

class Triangle : public Primitive
{
private:
	Vector4D	m_vertices[3];
	Vector4D	m_vertexNormals[3];
	Vector4D	m_texCoords[3];
	Vector4D	m_normal;
	Vector4D	m_edge1;
	Vector4D	m_edge2;
	bool		normalsSet = false;

public:
	Triangle();
	Triangle(Vector4D pos1, Vector4D pos2, Vector4D pos3);
	~Triangle();
	
	void SetTriangle(Vector4D v0, Vector4D v1, Vector4D v2);

	void SetNormals(Vector4D n1, Vector4D n2, Vector4D n3);
	void SetTexCoords(Vector4D tc1, Vector4D tc2, Vector4D tc3);

	inline Vector4D GetNormal()
	{
		return m_normal;
	}

	inline Vector4D GetNormal() const
	{
		return m_normal;
	}

	inline Vector4D* GetVertices() const
	{
		return (Vector4D*)m_vertices;
	}
	void ApplyMatrix(const Matrix4x4 &trans);
	void ApplyScale(double scale);
	RayHitResult IntersectByRay(Ray& ray);
};

