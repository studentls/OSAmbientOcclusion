// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de


#ifndef VECTOR_HEADER_
#define VECTOR_HEADER_

#include <cmath>
#include <string>
#include <sstream>

#define EPSILON 0.00001

class Vector
{
public:
	float x;
	float y;
	float z;
	//Added 4th Vector variable for SSE purposes
	float w;	

	//Default constructor
	Vector()		{x = y = z = 0.0; w = 1.0;}

	//Copy constructor
	Vector(const Vector& v):x(v.x), y(v.y), z(v.z), w(v.w)	{}

	//value constructor
	Vector(const float _x, const float _y,
			const float _z, const float _w):x(_x), y(_y), z(_z), w(_w)	{}
	//value constructor 2
	Vector(const float _x, const float _y, const float _z):x(_x), y(_y), z(_z), w(1.0)	{}

	//value construcor 3
	Vector(const float _x, const float _y):x(_x), y(_y), z(0.0), w(1.0)	{}

	//Array construcor
	Vector(const float *_av):x(_av[0]),y(_av[1]), z(_av[2]), w(1.0)	{}

	//set Zero
	inline void setZero()	{x = y = z = 0.0; w = 1.0;}

	//set to component
	inline void set(float _x, float _y, float _z)	{x = _x; y = _y; z = _z; w = 1.0;}

	//component getter methods
	inline float getX()	{return x;}
	inline float getY()	{return y;}
	inline float getZ()	{return z;}
	inline float getW()	{return w;}

	//component setter methods
	inline void setX(float _x)	{x = _x;}
	inline void setY(float _y)	{x = _y;}
	inline void setZ(float _z)	{x = _z;}
	inline void setW(float _w)	{x = _w;}

	//--------------------------------------
	//Methods

	inline float		scalarproduct(const Vector& v)		{return x * v.x + y * v.y + z * v.z;}
	inline Vector		crossproduct(const Vector& v)		{return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);}
	inline float		getLength()
	{
		float scalar = x * x + y * y + z * z;
		return sqrt(scalar);
		//return d;

	}
	inline Vector		plus(const Vector& v)	{return Vector(x + v.x, y + v.y, z + v.z);}
	inline Vector		minus(const Vector& v)	{return Vector(x - v.x, y - v.y, z - v.z);}
	inline Vector		times(const float d)		{return Vector(x * d, y * d, z * d);}
	inline Vector		getNormalized()
	{
		float l = getLength();
		//secure in debug mode, avoid division by zero!
#ifdef _DEBUG
		if(l == 0.0)return Vector();
#endif
		return Vector(x / l, y / l, z / l);
	}
	inline void			normalize()
	{
		float l = getLength();
		//secure in debug mode, avoid division by zero!
#ifdef _DEBUG
		if(l == 0.0)*this = Vector();
#endif	
		x /= l; y /= l; z /= l;
	}
	inline std::string toString()
	{
		std::stringstream s;
		s<<"("<<x<<","<<y<<","<<z<<")";
		return s.str();
	}
	inline bool equals(Vector& v)
	{
		float eps = EPSILON;//0.0000001;
		Vector diff = this->minus(v);
		return (abs(diff.x) < eps && abs(diff.y) < eps && abs(diff.z) < eps);
	}

	inline bool operator == (const Vector& v)
	{
		float eps = EPSILON;//0.0000001;
		Vector diff = this->minus(v);
		return (abs(diff.x) < eps && abs(diff.y) < eps && abs(diff.z) < eps);
	}

	void operator =		(const Vector& v) {x =  v.x; y =  v.y; z =  v.z;}
	void operator +=	(const Vector& v)	{x += v.x; y += v.y; z += v.z;}
	void operator -=	(const Vector& v) {x -= v.x; y -= v.y; z -= v.z;}
	void operator *=	(const float& f)	{x *= f;   y *= f;   z *= f;  }
	void operator /=	(const float& f)	{x /= f;   y /= f;   z /= f;  }

	static const Vector INFINITY;
};

//operators
inline Vector operator + (const Vector& a, const Vector& b){return Vector(a.x + b.x, a.y + b.y, a.z + b.z);}
inline Vector operator - (const Vector& a, const Vector& b){return Vector(a.x - b.x, a.y - b.y, a.z - b.z);}
inline Vector operator - (const Vector& v)					  {return Vector(-v.x, -v.y, -v.z);}
inline Vector operator * (const float d, const Vector& v)	  {return Vector(v.x * d, v.y * d, v.z * d);}
inline Vector operator * (const Vector& v, const float d)	  {return Vector(v.x * d, v.y * d, v.z * d);}
inline float   operator * (const Vector& a, const Vector& b){return a.x * b.x + a.y * b.y + a.z * b.z;}
inline Vector operator / (const float d, const Vector& v)	  {return Vector(v.x / d, v.y / d, v.z / d);}
inline Vector operator / (const Vector& v, const float d)	  {return Vector(v.x / d, v.y / d, v.z / d);}


//Inline funktion
inline float	VectorLength(const Vector& v)
{
	return sqrt(v.x * v.x +  v.y * v.y + v.z * v.z);
}

inline Vector	VectorAbs(const Vector& v)
{
	return (v.x < 0 || v.y < 0 || v.z < 0) ? -v : v;
}

inline Vector	VectorInterpolate(const Vector& A, const Vector& B,
	const Vector& C, const Vector& D, const float x, const float y)
{
	Vector P(A + x * (B - A));
	Vector Q(C + x * (D - C));
	return P + y * (Q - P);
}

inline Vector VectorMax(const Vector& a, const Vector& b)
{
	Vector res = a;
	if(b.x > res.x)res.x = b.x;
	if(b.y > res.y)res.y = b.y;
	if(b.z > res.z)res.z = b.z;
	return res;
}

inline Vector VectorMin(const Vector& a, const Vector& b)
{
	Vector res = a;
	if(b.x < res.x)res.x = b.x;
	if(b.y < res.y)res.y = b.y;
	if(b.z < res.z)res.z = b.z;
	return res;
}

#endif
