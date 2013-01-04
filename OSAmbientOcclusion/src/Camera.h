// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de


#ifndef CAMERA_HEADER_
#define CAMERA_HEADER_

#include "Ray.h"


class Camera
{
private:
	/// camera vectors
	Vector pos;
	Vector view;
	Vector up;
	Vector right;

	/// vertical field of view
	float fovy;

	/// 4 vectors forming the frustrum
	Vector m_vEdges[4];

	float viewPlaneHeight;
	float viewPlaneWidth;

	float width;
	float height;
	
	inline Ray calcRay(float fX, float fY)
	{
		double offsetX = (double)viewPlaneWidth
				* (fX / (double) width - 0.5f + 0.5f / (double)width);
		double offsetY = -(double)viewPlaneHeight
				* (fY / (double) height - 0.5f + 0.5f / (double)height);

		Vector viewPlaneOffX = right * (offsetX);
		Vector viewPlaneOffY = up * (offsetY);
			
		Vector dir = view + (viewPlaneOffX) + (viewPlaneOffY);

		return Ray(pos, dir);

	}

public:

	inline Ray getRay(float fX, float fY)
	{
		float x = fX * 1.0f / (float)width;
		float y = fY * 1.0f / (float)height;

		Vector dir = VectorInterpolate(m_vEdges[0], m_vEdges[1], m_vEdges[2], m_vEdges[3], x, y);

		return Ray(pos, dir);
	}


	void	setPositionAndLookAt(const float fovY, const Vector camPos, const Vector lookAt, const Vector upDir, const int imageWidth, const int imageHeight)
	{
		view = lookAt - camPos;
		view.normalize();

		right = view.crossproduct(upDir).getNormalized();
		up = right.crossproduct(view).getNormalized();

		// initialize image
		width = imageWidth;
		height = imageHeight;

		//field of view y-direction
		fovy = fovY;

		// compute height and width for a viewPlane at distance 1.0f
		viewPlaneHeight = 2.0 * tan(fovy / 2.0);
		viewPlaneWidth = (viewPlaneHeight * width) / height;

		//added! init rays!
		m_vEdges[0] = calcRay(0, 0).direction;
		m_vEdges[1] = calcRay(width, 0).direction;
		m_vEdges[2] = calcRay(0, height).direction;
		m_vEdges[3] = calcRay(width, height).direction;

	}

};


#endif