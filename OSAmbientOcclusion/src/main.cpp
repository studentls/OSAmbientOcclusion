// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de

#include "main.h"

using namespace std;


// global image
Image g_image;

// list of scene objects
vector<IObject*> g_objects;

// list of scene lights
vector<ILight*> g_lights;

// camera
Camera g_camera;

void Draw()
{
	// draw fullscreen quad
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_image.getTexture());


	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3i(-1, -1, -1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3i(1, -1, -1);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3i(1, 1, -1);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3i(-1, 1, -1);
	glEnd ();


	glDisable(GL_TEXTURE_2D);

	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}


Color shade(const Ray& r, const float fDistance, const Vector& normal, const Color& color)
{
	// shade with lights
	Color res = color;

	if(!g_lights.empty())
	{
		res = Color::black;

		for(vector<ILight*>::iterator it = g_lights.begin();
			it != g_lights.end(); ++it)
		{
			res = res + (*it)->shade(r, fDistance, normal, color);
		}
	}

	return res;
}

bool intersectObjects(const Ray& r, float& fDistance, Vector& normal, Color& color)
{
	// simply intersect with all objects...
	if(g_objects.empty())return false;

	float fLastDistance = 99999.9f;
	color = Color::white;
	bool intersection = false;

	for(vector<IObject*>::iterator it = g_objects.begin();
		it != g_objects.end(); ++it)
	{
		// temp variables
		Vector _normal;
		Color _color;
		if((*it)->intersect(r, fDistance, _normal, _color))
		{
			intersection = true;

			// nearer?
			if(fDistance < fLastDistance)
			{
				fLastDistance = fDistance;
				color = _color;
				normal = _normal;
			}
		}
	}

	return intersection;
}

Color traceRay(const Ray& r)
{
	
	Vector normal;
	Color color;
	float fDistance;

	// shade
	if(intersectObjects(r, fDistance, normal, color))color = shade(r, fDistance, normal, color);

	return color;
}

void Raytrace()
{
	// raytrace...
	for(int x = 0; x < g_width; x++)
		for(int y = 0; y < g_height; y++)
		{
			// trace ray
			Ray ray = g_camera.getRay(x, y);

			Color col = traceRay(ray);

			g_image.setPixel(x, y, col);
		}
}

void createScene()
{
	// some new things
	///fov = 3.14159 / 3.2
	Vector camPos	= Vector(0, -0.7, -0.3);
	Vector lookAt	= Vector(0, -0.7, -6);
	Vector upDir	= Vector(0, 1, 0);
	g_camera.setPositionAndLookAt(3.14159 / 3.2,
		camPos, lookAt, upDir, g_width, g_height);

	// add some spheres

	Sphere *sph1 = new Sphere(1.0f, Vector(0.75, -1, -4.75), Color::yellow);
	Sphere *sph2 = new Sphere(0.75f, Vector(-0.75, -1.25, -3.5), Color::blue);
	Sphere *sph3 = new Sphere(0.4f, Vector(0.5, -1.6, -3.5), Color::green);
	g_objects.push_back(sph1);
	g_objects.push_back(sph2);
	g_objects.push_back(sph3);

	// add some boxes
	Box *box1 = new Box(Vector(-2, -2, -6), Vector(2, 2, 0.1),
				Color::white * 0.8f);
	g_objects.push_back(box1);

	// add some lights
	AmbientLight *alight = new AmbientLight(0.2f * Color::white);
	DirectionalLight *dirlight = new DirectionalLight(Color::white, -Vector(0.0f, 1.0f, 0.6f));

	g_lights.push_back(alight);
	g_lights.push_back(dirlight);
}

void deleteScene()
{
	// delete memory
	if(!g_objects.empty())
		for(vector<IObject*>::iterator it = g_objects.begin();
			it != g_objects.end(); ++it)
		{
			delete *it;
			*it = NULL;
		}

	g_objects.clear();

	// delete memory
	if(!g_lights.empty())
		for(vector<ILight*>::iterator it = g_lights.begin();
			it != g_lights.end(); ++it)
		{
			delete *it;
			*it = NULL;
		}

	g_lights.clear();
}

int main(int argc, char * argv[])
{
	// set up image
	g_image.create(g_width, g_height);

	// put some pixels
	for(int i = 0; i < min(g_width, g_height); i++)
	{
		g_image.setPixel(i, i, Color::white);
	}

	// define some scene objects
	createScene();

	//raytrace Image
	Raytrace();
	
	// init glfw
	glfwInit();


	// open window
	glfwOpenWindow(g_width, g_height, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);
	glfwSetWindowTitle("Object Space Ambient Occlusion");

	// loop till end
	bool running = true;
	while(running)
	{

		Draw();

		// swap buffers
		glfwSwapBuffers();

		// if ESC or window closed terminate
		running = ! glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	//terminate glfw
	glfwTerminate();


	// delete objects
	deleteScene();

	return 0;
}