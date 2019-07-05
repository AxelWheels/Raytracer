#pragma once
#include <math.h>
#define PI 3.14159265359
#define DEG_TO_RAD(x) x * PI / 180.0f
class Vector4D;

class Matrix4x4
{
private:
	double				m_element[4][4];
	double				r[4][4];
	double				camPos[3];

public:
	Matrix4x4();
	Matrix4x4(const Matrix4x4& rhs);
	~Matrix4x4();

	void				SetZero();
	void				SetIdentity();
	const double*		ToPtr() const;
	
	//The following methods are not implemented
	//This is a part of the basic requirements for Coursework 2
	Matrix4x4			operator*(const Matrix4x4& rhs) const;
	Matrix4x4			operator=(const Matrix4x4& rhs);
	Vector4D			operator*(const Vector4D& rhs) const;
	
	void				SetRotationXAxis(float degrees);
	void				SetRotationYAxis(float degrees);
	void				SetRotationZAxis(float degrees);
	void				SetTranslate(const Vector4D& tvec);
	void				Transpose();
	void				InvertMatrix();
	void				SetScale(float sx, float sy, float sz);
	void				SetViewMatrix(const Vector4D& camera_position, const Vector4D& view_vector, const Vector4D& up_vector);
	void				SetCameraPosition(float x, float y, float z);
};

