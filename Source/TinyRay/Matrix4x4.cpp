#include <memory.h>
#include "Vector4D.h"
#include "Matrix4x4.h"

Matrix4x4::Matrix4x4()
{
	SetIdentity();
	camPos[0] = 0;
	camPos[1] = 0;
	camPos[2] = 0;
}

Matrix4x4::Matrix4x4(const Matrix4x4& rhs)
{
	size_t matrix_size = sizeof(m_element);

	memcpy_s((&m_element[0][0]), matrix_size, rhs.ToPtr(), matrix_size);
}

Matrix4x4::~Matrix4x4()
{

}

const double* Matrix4x4::ToPtr() const
{
	return &(m_element[0][0]);
}

void Matrix4x4::SetZero()
{
	memset(&m_element, 0, sizeof(m_element));
}

void Matrix4x4::SetIdentity()
{
	SetZero();
	m_element[0][0] = m_element[1][1] = m_element[2][2] = m_element[3][3] = 1.0f;
}

Matrix4x4 Matrix4x4::operator* (const Matrix4x4& rhs) const
{
	Matrix4x4 result;
	float temp = 0.00f;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				temp += (m_element[i][0 + k] * rhs.m_element[0+k][j]);
			}
			result.m_element[i][j] = temp;
			temp = 0.00f;
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::operator=(const Matrix4x4 & rhs)
{
	camPos[0] = rhs.camPos[0];
	camPos[1] = rhs.camPos[1];
	camPos[2] = rhs.camPos[2];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_element[i][j] = rhs.m_element[i][j];
			r[i][j] = rhs.r[i][j];
		}
	}

	return *this;
}

Vector4D Matrix4x4::operator* (const Vector4D& rhs) const
{
	Vector4D result;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += (m_element[i][j] * rhs[j]);
		}
	}
	
	return result;
}

void Matrix4x4::SetRotationXAxis(float degrees)
{
	SetIdentity();
	degrees = DEG_TO_RAD(degrees);

	m_element[1][1] = cos(degrees);
	m_element[1][2] = -(sin(degrees));
	m_element[2][1] = sin(degrees);
	m_element[2][2] = cos(degrees);
	//TODO: Set this matrix as a rotation matrix representing a rotation about the X axis by degrees
}

void Matrix4x4::SetRotationYAxis(float degrees)
{
	SetIdentity();
	degrees = DEG_TO_RAD(degrees);

	m_element[0][0] = cos(degrees);
	m_element[0][2] = -(sin(degrees));
	m_element[2][0] = sin(degrees);
	m_element[2][2] = cos(degrees);
	//TODO: Set this matrix as a rotation matrix representing a rotation about the Y axis by degrees
}

void Matrix4x4::SetRotationZAxis(float degrees)
{
	SetIdentity();
	degrees = DEG_TO_RAD(degrees);

	m_element[0][0] = cos(degrees);
	m_element[0][1] = -(sin(degrees));
	m_element[1][0] = sin(degrees);
	m_element[1][1] = cos(degrees);
	//TODO: Set this matrix as a rotation matrix representing a rotation about the Z axis by degrees
}

void Matrix4x4::SetTranslate(const Vector4D& tvec)
{
	SetIdentity();

	m_element[0][3] = tvec[0];
	m_element[1][3] = tvec[1];
	m_element[2][3] = tvec[2];
}

void Matrix4x4::Transpose()
{
	r[0][0] = m_element[0][0]; r[0][1] = m_element[1][0]; r[0][2] = m_element[2][0]; r[0][3] = m_element[3][0];
	r[1][0] = m_element[0][1]; r[1][1] = m_element[1][1]; r[1][2] = m_element[2][1]; r[1][3] = m_element[3][1];
	r[2][0] = m_element[0][2]; r[2][1] = m_element[1][2]; r[2][2] = m_element[2][2]; r[2][3] = m_element[3][2];
	r[3][0] = m_element[0][3]; r[3][1] = m_element[1][3]; r[3][2] = m_element[2][3]; r[3][3] = m_element[3][3];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_element[i][j] = r[i][j];
		}
	}
	
}

void Matrix4x4::SetScale(float sx, float sy, float sz)
{
	SetIdentity();

	m_element[0][0] = sx;
	m_element[1][1] = sy;
	m_element[2][2] = sz;
}


void Matrix4x4::SetViewMatrix(const Vector4D& camera_position, const Vector4D& view_vector, const Vector4D& up_vector)
{
	//TODO: Slightly more advanced
	//Set this matrix as a view matrix based on the given camera_position, view_vector and up_vector
	SetIdentity();
	Vector4D right_vector = view_vector.CrossProduct(up_vector);
	right_vector.Normalise();
	for (int i = 0; i < 3; i++)
	{
		m_element[0][i] = right_vector[i];
		m_element[1][i] = up_vector[i];
		m_element[2][i] = view_vector[i];
	}
}

void Matrix4x4::SetCameraPosition(float x, float y, float z)
{
	m_element[0][3] = camPos[0] = x;
	m_element[1][3] = camPos[1] = y;
	m_element[2][3] = camPos[2] = z;
}

void Matrix4x4::InvertMatrix()
{
//http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
	double inv[16], det;
	double m[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i * 4 + j] = m_element[i][j];
		}
	}
	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return;

	det = 1.0 / det;

	for (int i = 0; i < 16; i++)
	{
		m[i] = inv[i] * det;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_element[i][j] = m[i * 4 + j];
		}
	}
}
