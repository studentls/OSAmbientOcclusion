// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de

#ifndef LIGHTS_HEADER_
#define LIGHTS_HEADER_

#include "Ray.h"
#include "Color.h"

class ILight
{
private:
public:
	virtual Color shade(const Ray& r, const float fDistance, const Vector& normal, const Color& color)=0;

};

// simple ambient light
class AmbientLight : public ILight
{
private:
	Color col;
public:
	AmbientLight(const Color& _col):col(_col) {}

	Color shade(const Ray& r, const float fDistance, const Vector& normal, const Color& color)
	{
		// simply multiply colors
		return col * color;
	}

};

// directional light
class DirectionalLight : public ILight
{
private:
	Color col;
	Vector dir;
public:
	DirectionalLight(const Color& _col, const Vector& _dir):col(_col), dir(_dir)	{dir.normalize();}

	// shade
	Color shade(const Ray& r, const float fDistance, const Vector& normal, const Color& color)
	{
		// simple diffuse shading, without specularity
		
		Vector n = normal;
		if (n * r.direction > 0.0)
			n *= -1.0;

		// diffuse illumination
		float factorDiff = n * dir * -1.0;
		if (factorDiff < 0.0)
			return Color(0.0f, 0.0, 0.0);

		Color c = color * ( col * factorDiff) ;

		return c;
	}
};

#endif