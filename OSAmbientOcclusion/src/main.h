// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de

#ifndef MAIN_HEADER_
#define MAIN_HEADER_

// needs boost & glfw installed to compile properly

#include <iostream>
#include <boost/thread.hpp>
#include <gl/glfw.h>

// own headers
#include "Image.h"
#include "Color.h"
#include "Ray.h"
#include "Objects.h"
#include "Camera.h"
#include "Lights.h"
#include "Matrix.h"

// size of render window
static const int g_width = 500;
static const int g_height = 500;

/// float random function
float random(float fmin, float fmax)
{
	return fmin + ((float)(rand()%RAND_MAX) / (float)RAND_MAX) / (fmax - fmin);
}

#endif