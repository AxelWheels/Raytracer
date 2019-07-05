//Created for Graphics I and II
//Author: Minsi Chen

#include "TriangleMesh.h"
#include "OBJFileReader.h"
#include "Material.h"

TriangleMesh::TriangleMesh()
{
	m_triangles = NULL;
	m_numTriangles = 0;
	m_primtype = PRIMTYPE_Triangle;
}

TriangleMesh::TriangleMesh(const char * filename)
{
	m_triangles = NULL;
	m_numTriangles = 0;

	//Transformations for object
	m_scale.SetScale(1,1,1);
	m_trans.SetTranslate(Vector4D(3.0, 3.0, -1.0));
	m_trans = m_trans * m_scale;
	//Load in an OBJ file with either just vertices or vertices, normals and texture coordinates
	LoadMeshFromOBJFile(filename);

	//Bounding box vertices
	m_bottomLeft = m_triangles[0].GetVertices()[0];
	m_topRight = m_triangles[0].GetVertices()[0];
	m_primtype = PRIMTYPE_Triangle;

	//Load in a texture
	if (ImageReader::LoadTGA("house_diffuse.tga", &buffer, &sizeX, &sizeY, &bpp, &nChannels))
	{
		hasTexture = true;
	}

	//Set bounding box vertices
	for (int i = 0; i < m_numTriangles; i++)
	{
		m_triangles[i].ApplyMatrix(m_trans);
		Vector4D *points = m_triangles[i].GetVertices();
		for (int j = 0; j < 3; j++)
		{
			//bottom left of bounding box
			if (points[j][0] < m_bottomLeft[0])
			{
				m_bottomLeft[0] = points[j][0];
			}
			if (points[j][1] < m_bottomLeft[1])
			{
				m_bottomLeft[1] = points[j][1];
			}
			if (points[j][2] < m_bottomLeft[2])
			{
				m_bottomLeft[2] = points[j][2];
			}

			//Top right of bounding box
			if (points[j][0] > m_topRight[0])
			{
				m_topRight[0] = points[j][0];
			}
			if (points[j][1] > m_topRight[1])
			{
				m_topRight[1] = points[j][1];
			}
			if (points[j][2] >  m_topRight[2])
			{
				m_topRight[2] = points[j][2];
			}
		}	
	}
}


TriangleMesh::~TriangleMesh()
{
	CleanUp();
}

RayHitResult TriangleMesh::IntersectByRay(Ray & r)
{
	RayHitResult result;
	result = Ray::s_defaultHitResult;

	if (intersectBox(r))
	{
		for (int i = 0; i < m_numTriangles; i++)
		{
			RayHitResult temp = m_triangles[i].IntersectByRay(r);
			if (temp.t < result.t)
			{
				int x = (temp.UV[0] * sizeX);
				int y = (temp.UV[1] * sizeY);
				int p = (y * sizeX + x) * 4;

				temp.texColour = { buffer[p] / 255.0f, buffer[p + 1] / 255.0f, buffer[p + 2] / 255.0f };

				temp.data = this;
				result = temp;
			}
		}
	}
	return result;
}

bool TriangleMesh::LoadMeshFromOBJFile(const char* filename)
{
	m_numTriangles = OBJFileReader::ImportObjMesh(filename, &m_triangles);

	return true;
}

bool TriangleMesh::intersectBox(Ray & r)
{
	//http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
	// r.GetRay is unit direction vector of ray
	double dirfracx = 1.0f / r.GetRay()[0];
	double dirfracy = 1.0f / r.GetRay()[1];
	double dirfracz = 1.0f / r.GetRay()[2];
	// m_bottomLeft is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.GetRayStart is origin of ray
	float t1 = (m_bottomLeft[0] - r.GetRayStart()[0])*dirfracx;
	float t2 = (m_topRight[0] - r.GetRayStart()[0])*dirfracx;
	float t3 = (m_bottomLeft[1] - r.GetRayStart()[1])*dirfracy;
	float t4 = (m_topRight[1] - r.GetRayStart()[1])*dirfracy;
	float t5 = (m_bottomLeft[2] - r.GetRayStart()[2])*dirfracz;
	float t6 = (m_topRight[2] - r.GetRayStart()[2])*dirfracz;

	float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
	float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		return false;
	}

	return true;
}

void TriangleMesh::SetTrans(const Matrix4x4 * transformation)
{
	m_trans = *transformation;
}

void TriangleMesh::CleanUp()
{
	if (m_triangles)
		delete[] m_triangles;

	m_triangles = NULL;
	m_numTriangles = 0;
}