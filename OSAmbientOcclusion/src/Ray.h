// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de

#ifndef RAY_HEADER_
#define RAY_HEADER_

#include "Vector.h"


class Ray
{
public:
	Vector	origin;
	Vector	direction;

	//default constructor
	Ray()
	{
		origin = Vector();//Vector::INFINITY;
		direction = Vector();
	}

	//constructor
	Ray(const Vector& _origin, Vector _direction)
	{
		origin = _origin;
		direction = _direction.getNormalized();
	}

	//copy constructor
	Ray(const Ray& r)
	{
		origin = r.origin;
		direction = r.direction;
	}

	void operator =		(const Ray& r) {origin = r.origin;  direction = r.direction;}
};


#endif
