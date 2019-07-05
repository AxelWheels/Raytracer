//Created for Graphics I and II
//Author: Minsi Chen

#include "OBJFileReader.h"
#include "Vector4D.h"

int OBJFileReader::firstPassOBJRead(const char* filename, int* vertex_count, int* vert_normal_count, int* vert_texcoord_count, int* triangle_count)
{
	FILE* pfile;
	errno_t err;

	*vertex_count = 0;
	*vert_normal_count = 0;
	*vert_texcoord_count = 0;
	*triangle_count = 0;

	err = fopen_s(&pfile, filename, "r");

	if (!pfile)
	{
		//something has gone wrong when opening the file.
		return 1;
	}

	char tempbuffer[1024];
	char* strread = fgets(tempbuffer, 1024, pfile);

	do
	{
		char* v = &(tempbuffer[0]);
		if (*v == 'v')
		{
			if (*(v + 1) == 'n')		
				*vert_normal_count += 1;
			else if (*(v + 1) == 't')	
				*vert_texcoord_count += 1;
			else						
				*vertex_count += 1;
		}
		else if (*v == 'f')
		{
			*triangle_count += 1;
		}

		strread = fgets(tempbuffer, 1024, pfile);

	} while (strread != NULL);

	fclose(pfile);

	return 0;
}

int OBJFileReader::secondPassOBJRead(const char* filename, int nVerts, int nNormals, int nTexcoords, Triangle* inTriangles)
{
	FILE* pfile;
	errno_t err;
	err = fopen_s(&pfile, filename, "r");

	if (!pfile)
	{
		//something has gone wrong when opening the file.
		return 1;
	}

	char tempbuffer[1024];
	char* strread = fgets(tempbuffer, 1024, pfile);
	int vertex_read = 0;
	int normal_read = 0;
	int texcoord_read = 0;
	int triangle_read = 0;

	Vector4D *normal_buffer = new Vector4D[nNormals];
	Vector4D *vertex_buffer = new Vector4D[nVerts];
	Vector4D *texcoord_buffer = new Vector4D[nTexcoords];

	do
	{
		char* v = &(tempbuffer[0]);
		if (*v == 'v')
		{
			if (*(v + 1) == 'n')
			{
				sscanf_s(v, "%*s %Lf %Lf %Lf", &(normal_buffer[normal_read][0]),
					&(normal_buffer[normal_read][1]),
					&(normal_buffer[normal_read][2]));
				normal_read += 1;
			}
			else if (*(v + 1) == 't')
			{
				sscanf_s(v, "%*s %Lf %Lf", &(texcoord_buffer[texcoord_read][0]),
					&(texcoord_buffer[texcoord_read][1]));
				texcoord_read += 1;
			}
			else
			{
				sscanf_s(v + 1, "%Lf %Lf %Lf", &(vertex_buffer[vertex_read][0]),
					&(vertex_buffer[vertex_read][1]),
					&(vertex_buffer[vertex_read][2]));
				vertex_read += 1;
			}
		}
		else if (*v == 'f')
		{
			int dummy;
			int index1;
			int index2;
			int index3;
			int index4;

			int nidx1, nidx2, nidx3, nidx4;

			int tidx1, tidx2, tidx3, tidx4;

			if (sscanf_s(v, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &index1, &tidx1, &nidx1,
				&index2, &tidx2, &nidx2,
				&index3, &tidx3, &nidx3) == 9)
			{
				inTriangles[triangle_read].SetTriangle(vertex_buffer[index1 - 1], vertex_buffer[index2 - 1], vertex_buffer[index3 - 1]);
				inTriangles[triangle_read].SetNormals(normal_buffer[nidx1 - 1], normal_buffer[nidx2 - 1], normal_buffer[nidx3 - 1]);
				inTriangles[triangle_read].SetTexCoords(texcoord_buffer[tidx1 - 1], texcoord_buffer[tidx2 - 1], texcoord_buffer[tidx3 - 1]);
			}
			else if (sscanf_s(v, "%*s %d %d %d", &index1, &index2, &index3) == 3)
			{
				inTriangles[triangle_read].SetTriangle(vertex_buffer[index1 - 1], vertex_buffer[index2 - 1], vertex_buffer[index3 - 1]);
			}
			else
			{
				break;
			}
			triangle_read += 1;
		}

		strread = fgets(tempbuffer, 1024, pfile);

	} while (strread != NULL);

	fclose(pfile);

	delete[] vertex_buffer;
	delete[] normal_buffer;
	delete[] texcoord_buffer;

	return 0;
}

int OBJFileReader::ImportObjMesh(const char * filename, Triangle ** triangles)
{
	int num_triangles = 0;
	int num_vertices = 0;
	int num_normals = 0;
	int num_texcoords = 0;

	if (firstPassOBJRead(filename, &num_vertices, &num_normals, &num_texcoords, &num_triangles)==1)
		return 0;

	*triangles = new Triangle[num_triangles];

	if (secondPassOBJRead(filename, num_vertices, num_normals, num_texcoords, *triangles)==1)
		return 0;

	return num_triangles;
}
