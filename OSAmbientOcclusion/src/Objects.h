// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de

#ifndef OBJECTS_HEADER_
#define OBJECTS_HEADER_

#include "Ray.h"
#include "Color.h"

// file contains some simple objects, used to to perform intersection routine

/// asbstract object class
class IObject
{
private:
public:
	/// intersect with ray, output distance, color, tangent
	virtual bool intersect(const Ray& r, float& fDistance, Vector& normal, Color& color) = 0;
};

// two Objects
class Sphere : public IObject
{
private:
	float	radius;
	Vector	center;
	Color	color;
public:

	Sphere():radius(0.0f)	{}

	Sphere(const float _radius, const Vector& _center):radius(_radius), center(_center), color(Color::white)	{}

	Sphere(const float _radius, const Vector& _center, const Color& _color):radius(_radius), center(_center), color(_color)	{}

	
	virtual bool intersect(const Ray& r, float& fDistance, Vector& normal, Color& color)
	{
		double radius2 = radius * radius;
		Vector center2origin = r.origin - (center);
		double a = r.direction * (r.direction);
		double b = 2 * (center2origin.scalarproduct(r.direction));
		double c = center2origin.scalarproduct(center2origin) - radius2;

		//discriminant
		double d = b * b - 4.0 * a * c;
		double t;	//the solution

		//negative?
		if(d < 0.0)return false;

		//compute root
		double dSqrt = sqrt(d);
		double q;
		if(b < 0.0)q = (-b - dSqrt) / 2.0;
		else q = (-b + dSqrt) / 2.0;

		//compute t0/t1
		double t0 = q / a;
		double t1 = c / q;

		//t0 shall be smaller than t1
		if(t0 > t1)
		{
			double h = t0;
			t0 = t1;
			t1 = h;
		}

		//less than zero?
		if(t1 < 0.0)return false;

		//if t0 is less than zero intersection will be at t1
		if(t0 < 0.0)t = t1; 
		else t = t0;

		//calc rtIntersection
		Vector intPos = r.origin + (r.direction * t);
		Vector n = intPos - center;

		n.normalize();
		//return rtIntersection(ray, intPos, normal, material);
		
		// out data
		fDistance = t;
		normal = n;
		color = this->color;
		
		return true;
	}
};


// a box
class Box : public IObject
{
private:
	Vector		center;
	Vector		normals[3];
	double		halfSize[3];

	Color		col;

	void setMinMax(Vector min, Vector max) {
		center = (max + min) * 0.5;
		Vector hVector = (max - min) * 0.5;
		halfSize[0] = hVector.getX();
		halfSize[1] = hVector.getY();
		halfSize[2] = hVector.getZ();
	}

public:
	Box()
	{	
		normals[0] = Vector(1, 0, 0);
		normals[1] = Vector(0, 1, 0);
		normals[2] = Vector(0, 0, 1);

		halfSize[0] = halfSize[1] = halfSize[2] = 0.0;
	}

	Box(Vector min, Vector max, Color col)
	{
		normals[0] =Vector(1, 0, 0);
		normals[1] =Vector(0, 1, 0);
		normals[2] =Vector(0, 0, 1);
		this->col = col;
		setMinMax(min, max);
	}

	virtual bool intersect(const Ray& r, float& fDistance, Vector& normal, Color& color)
	{
		double tmin = -99999.9f;
		double tmax = 99999.9f;

		int idmin = 0;
		int idmax = 0;

		Vector p = center.minus(r.origin);

		for (int i = 0; i < 3; i++) {
			double e = normals[i].scalarproduct(p);
			double f = normals[i].scalarproduct(r.direction);

			int id1 = i * 2;
			int id2 = i * 2 + 1;

			if (abs(f) > 0.0000000001) {
				double t1 = (e + halfSize[i]) / f;
				double t2 = (e - halfSize[i]) / f;

				if (t1 > t2) {
					double temp = t1;
					t1 = t2;
					t2 = temp;
					int tmp = id1;
					id1 = id2;
					id2 = tmp;
				}

				if (tmin < t1) {
					tmin = t1;
					idmin = id1;
				}
				if (tmax > t2) {
					tmax = t2;
					idmax = id2;
					if (tmax < 0)
						return false;
				}
				if (tmin > tmax)
					return false;
			} else if (-e - halfSize[i] > 0 || -e + halfSize[i] < 0)
				return false;
		}
		double t;
		if (tmin > 0.0) {
			t = tmin;// box in front of camera
			if (idmin % 2 == 0)
				normal = normals[idmin / 2];
			else
				normal = normals[idmin / 2] * (-1.0);
		} else {
			t = tmax;// camera inside of box
			if (idmax % 2 == 0)
				normal = normals[idmax / 2];
			else
				normal = normals[idmax / 2] * (-1.0);
		}
		
		color = col;
		fDistance = t;

		return true;
	}
};

#endif