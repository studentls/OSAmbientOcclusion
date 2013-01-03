// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de


#ifndef COLOR_HEADER_
#define COLOR_HEADER_

#include "main.h"

static const double g_ColConv = 0.003921568627450980392156862745098;

/// Basic Color class
class Color
{
public:
	float a;
	float r;
	float g;
	float b;

	Color():r(0.0),g(0.0),b(0.0),a(1.0) {}
	Color(const float _r, const float _g, const float _b):r(_r), g(_g), b(_b), a(1.0)	{} 
	Color(const float _r, const float _g, const float _b, const float _a):r(_r), g(_g), b(_b), a(_a)	{} 
	Color(const int _r, const int _g, const int _b)
	{
		r = (float)_r / 255.0;
		g = (float)_g / 255.0;
		b = (float)_b / 255.0;
		a = 1.0;
	}
	Color(const Color& c):r(c.r), g(c.g), b(c.b), a(c.a)	{}

	/// value constructor out of unsigned long value
	Color(const unsigned long& col): r(g_ColConv * (float)(unsigned char)(col >> 16)),
								  g(g_ColConv * (float)(unsigned char)(col >> 8)),
								  b(g_ColConv * (float)(unsigned char)(col)),
								  a(g_ColConv * (float)(unsigned char)(col >> 24))	{}

	/// convert to standard 32 Bit representation 
	operator unsigned long ()
	{
		
		return((a >= 1.0 ? 255 : a <= 0.0 ? 0 : (unsigned long)(a * 255.0)) << 24 |
			   (r >= 1.0 ? 255 : r <= 0.0 ? 0 : (unsigned long)(r * 255.0)) << 16 |
			   (g >= 1.0 ? 255 : g <= 0.0 ? 0 : (unsigned long)(g * 255.0)) << 8 |
			   (b >= 1.0 ? 255 : b <= 0.0 ? 0 : (unsigned long)(b * 255.0)));
				
	}

	int getRed()	{return (r >= 1.0 ? 255 : r <= 0.0 ? 0 : (unsigned long)(r * 255.0)); }
	int getGreen()	{return (g >= 1.0 ? 255 : g <= 0.0 ? 0 : (unsigned long)(g * 255.0)); }
	int getBlue()	{return (b >= 1.0 ? 255 : b <= 0.0 ? 0 : (unsigned long)(b * 255.0)); }
	
	
	void operator =		(const Color& c) {r =  c.r; g =  c.g; b =  c.b; a = c.a;}

	inline std::string toString()
	{
		std::stringstream s;
		s<<"("<<r<<","<<g<<","<<b<<","<<a<<")";
		return s.str();
	}

	//to def!
	static const Color white;
	static const Color black;
};


//operators

/// multiply (alpha value will not be multiplied)
inline Color operator * (const Color& c, const float d)			{return Color(c.r * d, c.g * d, c.b * d); } 
inline Color operator * (const float d, const Color& c)			{return Color(c.r * d, c.g * d, c.b * d); } 
inline Color operator * (const Color& c, const Color& k)		{return Color(c.r * k.r, c.g * k.g, c.b * k.b);}
inline Color operator / (const Color& c, const float d)			{float invd = 1.0f / d; return Color(c.r * invd, c.g * invd, c.b * invd); } 
inline Color operator + (const Color& a, const Color& b)		{return Color(a.r + b.r, a.g + b.g, a.b + b.b);}
inline Color operator - (const Color& a, const Color& b)		{return Color(a.r - b.r, a.g - b.g, a.b - b.b);}
inline Color operator - (const Color& c)						{return Color(-c.r, -c.g, -c.b);}

#endif
