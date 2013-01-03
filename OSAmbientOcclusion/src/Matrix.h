// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de

#ifndef MATRIX_HEADER_
#define MATRIX_HEADER_

#include "Vector.h"


//Matrix class
class Matrix3x3
{
public:
	float m11, m12, m13;
	float m21, m22, m23;
	float m31, m32, m33;

	// identity
	Matrix3x3():
	m11(1.0f), m12(0.0f), m13(0.0),
	m21(0.0f), m22(1.0f), m23(0.0),
	m31(1.0f), m32(0.0f), m33(1.0)
	{
	}

	// column based constructor
	Matrix3x3(const Vector& _a1, const Vector& _a2, const Vector& _a3):
	m11(_a1.x), m21(_a1.y), m31(_a1.z),
	m12(_a2.x), m22(_a2.y), m32(_a2.z),
	m13(_a3.x), m23(_a3.y), m33(_a3.z)
	{
	}
};

inline Vector operator * (const Matrix3x3& m, const Vector& v)
{
	return Vector(m.m11 * v.x + m.m12 * v.y + m.m13 * v.z,
				  m.m21 * v.x + m.m22 * v.y + m.m23 * v.z,
				  m.m31 * v.x + m.m32 * v.y + m.m33 * v.z);				  
}

#endif