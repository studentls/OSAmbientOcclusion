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

// size of render window
static const int g_width = 640;
static const int g_height = 480;

#endif