//Created for Graphics I and II
//Author: Minsi Chen

#pragma once
#include "Matrix4x4.h"
#include "Triangle.h"
#include "Vector4D.h"
#include <vector>
#include "Primitive.h"
#include "ImageReader.h"
#include <math.h>

class TriangleMesh : public Primitive
{
	private:
		Vector4D		m_bottomLeft;
		Vector4D		m_topRight;
		Matrix4x4		m_trans;
		Matrix4x4		m_scale;
		Triangle*		m_triangles;		//a list of triangles
		int				m_numTriangles;		//the number of triangles in the mesh
		Matrix4x4		NormalOrientation;

		bool			hasTexture = false;
		unsigned char*	buffer;
		int				sizeX;
		int				sizeY;
		int				bufferSize;
		int				bpp;
		int				nChannels;

	public:
		TriangleMesh();
		TriangleMesh(const char* filename);
		~TriangleMesh();

		RayHitResult IntersectByRay(Ray& r);

		bool LoadMeshFromOBJFile(const char* filename);

		bool intersectBox(Ray& r);

		void SetTrans(const Matrix4x4 * transformation);

		void CleanUp();
};

