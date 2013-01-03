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

// normals
Image g_normals;

// ambient occlusion pass
Image g_aopass;

// any image needs a mutex
boost::mutex img_mutex;
boost::mutex norm_mutex;
boost::mutex ao_mutex;

// list of scene objects
vector<IObject*> g_objects;

// list of scene lights
vector<ILight*> g_lights;

// camera
Camera g_camera;

#define MAX_MODES 3
// mode
int mode;

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
	
	// bind texture according to mode
	if(mode == 0)
	{
		glBindTexture(GL_TEXTURE_2D, g_image.getTexture());
	}
	if(mode == 1)
	{
		glBindTexture(GL_TEXTURE_2D, g_aopass.getTexture());
	}
	if(mode == 2)
	{
		glBindTexture(GL_TEXTURE_2D, g_normals.getTexture());
	}


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
			if(abs(fDistance) < abs(fLastDistance))
			{
				fLastDistance = fDistance;
				color = _color;
				normal = _normal;
			}
		}
	}

	return intersection;
}

Color traceRay(const Ray& r, Vector& normal)
{
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
			Vector normal;

			Color col = traceRay(ray, normal);
			normal = -normal;

			// mutexes
			norm_mutex.lock();
			g_normals.setPixel(x, y, Color((normal.x + 1.0f) / 2.0f, (normal.y + 1.0f) / 2.0f, (normal.z + 1.0f) / 2.0f));
			norm_mutex.unlock();
			img_mutex.lock();			
			g_image.setPixel(x, y, col);
			img_mutex.unlock();
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
	Vector center = Vector(0.5 + 0.25, 1.6, -5.5);
	Box *box2 = new Box(center + Vector(0.25, 0.25, 0.25), center - Vector(0.25, 0.25, 0.25),
				Color::white * 0.8f);
	//g_objects.push_back(box1);
	//g_objects.push_back(box2);

	// add some lights
	AmbientLight *alight = new AmbientLight(0.2f * Color::white);
	DirectionalLight *dirlight = new DirectionalLight(Color::white, -Vector(0.2f, 1.0f, 0.6f));
	
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

Matrix3x3 getHemisphereTransformationMatrix(const Vector& normal)
{
	// first create a random ray for a standard oriented hemisphere
	Vector v = Vector(random(-1.0f, 1.0f), random(-1.0f, 1.0f), random(0.0f, 1.0f));

	// normalize
	v.normalize();

	// now transform

	// construct tangent and bitangent with Gram-Schmidt
	Vector tangent = v - normal * (normal * v);
	tangent.normalize();

	Vector bitangent = - tangent.crossproduct(normal);
	bitangent.normalize();

	return Matrix3x3(tangent, bitangent, normal);	
}

Vector getHemisphereVector(const Vector& normal)
{
	while(true)
	{
		float x = random(-1.0f, 1.0f);
		float y = random(-1.0f, 1.0f);
		float z = random(-1.0f, 1.0f);

		if(x*x + y*y + z*z > 1)continue; // not in the sphere contained

		if(Vector(x, y, z) * normal < 0)continue; // falsely oriented

		//sucess!
		Vector res = Vector(x, y, z);
		res.normalize();
		return res;
	}

	return Vector();
}

Color traceAO(const Ray& r)
{
	Color color = Color::black;

	// shoot randomly oriented rays from the hemisphere...
	int kernel_size = 64; // 64 samples
	float fDistance;
	Vector normal;
	Color col; //received color, dummy

	// intersection?
	if(intersectObjects(r, fDistance, normal, col))
	{
		// perform AO

		// determine intersection position
		Vector point = r.origin + (r.direction * fDistance);

		// shoot random rays
		Ray kernel_ray(point + 0.0001 * Vector(EPSILON, EPSILON, EPSILON), Vector()); // init with position

		// generate transformation matrix to reorient random vectors...
		Matrix3x3 m = getHemisphereTransformationMatrix(-normal);

		// dummies
		float kernel_fdist;
		Vector kernel_normal;
		Color kernel_color;

		// now perform AO
		float occlusion_factor = 0.0;
		for(int i = 0; i < kernel_size; i++)
		{
			// pick random vector
			Vector v = Vector(random(-1.0f, 1.0f), random(-1.0f, 1.0f), random(0.0f, -1.0f));

			// transform with matrix
			v = m * v;

			// generate ray
			//Vector v = getHemisphereVector(normal);

			kernel_ray.direction = v;

			// intersect with scene
			if(intersectObjects(kernel_ray, kernel_fdist, kernel_normal, kernel_color))
			{
				// range check
				if(kernel_fdist < 0.40f && kernel_fdist > 0.001f)occlusion_factor += 1.0; // simply add(maybe later account light better)
			}
		}

		occlusion_factor /= (float)kernel_size;
		color = Color(occlusion_factor, occlusion_factor, occlusion_factor);
	}

	return color;
}

void AmbientOcclusionPass()
{
	// raytrace...
	for(int x = 0; x < g_width; x++)
	{
		ao_mutex.lock();
		
		for(int y = 0; y < g_height; y++)
		{
			// trace ray
			Ray ray = g_camera.getRay(x, y);

			Color col = traceAO(ray);

			
			g_aopass.setPixel(x, y, col);
			
		}
		
		ao_mutex.unlock();
	}
}

/// own render thread
void RenderMain()
{
	//raytrace Image
	Raytrace();

	// perform AmbientOcclusion pass
	AmbientOcclusionPass();
}

int main(int argc, char * argv[])
{
	// set up images
	g_image.create(g_width, g_height);
	g_normals.create(g_width, g_height);
	g_aopass.create(g_width, g_height);

	// start mode is 0
	mode = 0;

	// put some pixels
	for(int i = 0; i < min(g_width, g_height); i++)
	{
		g_image.setPixel(i, i, Color::white);
	}

	// define some scene objects
	createScene();
	// start thread
	boost::thread renderThread(RenderMain);
	
	// init glfw
	glfwInit();
	
	// open window
	glfwOpenWindow(g_width, g_height, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);
	glfwSetWindowTitle("Object Space Ambient Occlusion");
	
	
	
	
	// loop till end
	bool running = true;
	while(running)
	{
		
		//lock mutexes
		img_mutex.lock();
		ao_mutex.lock();
		norm_mutex.lock();

		Draw();
		
		norm_mutex.unlock();
		ao_mutex.unlock();
		img_mutex.unlock();
		

		// swap buffers
		glfwSwapBuffers();

		// switch render modes...
		if(glfwGetKey(GLFW_KEY_F1)){mode = 0; img_mutex.lock(); g_image.forceupdate(); img_mutex.unlock();}
		if(glfwGetKey(GLFW_KEY_F2)){mode = 1; ao_mutex.lock(); g_aopass.forceupdate(); ao_mutex.unlock();}
		if(glfwGetKey(GLFW_KEY_F3)){mode = 2; norm_mutex.lock(); g_normals.forceupdate(); norm_mutex.unlock();}

		// if ESC or window closed terminate
		running = ! glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}



	//terminate glfw
	glfwTerminate();
	
	// wait for render thread
	renderThread.join();

	// delete objects
	deleteScene();

	return 0;
}