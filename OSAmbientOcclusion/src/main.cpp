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

// inverse ambient occlusion pass blurred
Image g_invao;

// final composited image
Image g_final;

// any image needs a mutex
boost::mutex img_mutex;
boost::mutex norm_mutex;
boost::mutex ao_mutex;
boost::mutex invao_mutex;
boost::mutex final_mutex;

// list of scene objects
vector<IObject*> g_objects;

// list of scene lights
vector<ILight*> g_lights;

// store positions
// store normals
class GBuffer
{
private:
	Vector *points;
	Vector *normals;
	int width, height;
public:
	GBuffer():points(NULL), normals(NULL)	{}

	GBuffer(const int _width, const int _height):width(_width), height(_height)
	{
		points = new Vector[width * height];
		normals = new Vector[width * height];
	}

	GBuffer(const GBuffer& buf)
	{
		width = buf.width;
		height = buf.height;

		points = new Vector[width * height];
		normals = new Vector[width * height];

		for(int i = 0; i < width*height; i++)
		{
			points[i] = buf.points[i];
			normals[i] = buf.normals[i];
		}
	}

	~GBuffer()
	{
		if(points)delete [] points;
		if(normals)delete [] normals;
	}

	void operator =		(const GBuffer& buf)
	{
		width = buf.width;
		height = buf.height;

		points = new Vector[width * height];
		normals = new Vector[width * height];

		for(int i = 0; i < width*height; i++)
		{
			points[i] = buf.points[i];
			normals[i] = buf.normals[i];
		}
	}

	Vector getPoint(const int x, const int y)
	{
		assert(0 <= x + y * width && x + y *width < width * height);

		return points[x + y * width];
	}

	Vector getNormal(const int x, const int y)
	{
		assert(0 <= x + y * width && x + y *width < width * height);

		return normals[x + y * width];
	}

	void setPoint(const int x, const int y, const Vector& point)
	{
		assert(0 <= x + y * width && x + y *width < width * height);
		points[x + y * width] = point;
	}

	void setNormal(const int x, const int y, const Vector& normal)
	{
		assert(0 <= x + y * width && x + y *width < width * height);
		normals[x + y * width] = normal;
	}
};


GBuffer g_GBuffer;

// camera
Camera g_camera;

#define MAX_MODES 5
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
	if(mode == 3)
	{
		glBindTexture(GL_TEXTURE_2D, g_invao.getTexture());
	}
	if(mode == 4)
	{
		glBindTexture(GL_TEXTURE_2D, g_final.getTexture());
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
			if(fDistance < fLastDistance)
			{
				fLastDistance = fDistance;
				color = _color;
				normal = _normal;
			}
		}
	}

	fDistance = fLastDistance;

	return intersection;
}

Color traceRay(const Ray& r, Vector& normal, Vector& point)
{
	Color color;
	float fDistance;

	// shade
	if(intersectObjects(r, fDistance, normal, color))color = shade(r, fDistance, normal, color);

	// calc point
	point = r.origin + fDistance * r.direction;

	return color;
}

// for antialiasing
Color traceGrid(const int x, const int y, const int grid_size)
{
	float fX = (float)x;
	float fY = (float)y;
	Color col = Color::black;

	float fdX = 1.0f / (float)grid_size;
	float fdY = 1.0f / (float)grid_size;

	Vector normal;
	Vector point;
	Ray ray;

	for(int i = 0; i < grid_size; i++)
		for(int j = 0; j < grid_size; j++)
		{
			ray = g_camera.getRay(fX - 0.5f + fdX * i, fY - 0.5f + fdY * j);
			col = col + traceRay(ray, normal, point);
		}

	//diff
	col = col / (float)(grid_size * grid_size);

	return col;
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
			Vector point;

			Color col = traceRay(ray, normal, point);
			
			// store in buffer
			g_GBuffer.setNormal(x, y, normal);
			g_GBuffer.setPoint(x, y, point);

			col = traceGrid(x, y, 5);

			// mutexes
			norm_mutex.lock();
			g_normals.setPixel(x, y, Color((normal.x + 1.0f) / 2.0f, (normal.y + 1.0f) / 2.0f, (normal.z + 1.0f) / 2.0f));
			norm_mutex.unlock();
			img_mutex.lock();			
			g_image.setPixel(x, y, col);
			img_mutex.unlock();
		}

	// generate depth picture
	float fminZ = 99999.9f, fmaxZ = -99999.9f;
	for(int x = 0; x < g_width; x++)
		for(int y = 0; y < g_height; y++)
		{
			fminZ = min(fminZ, g_GBuffer.getPoint(x, y).z);
			fmaxZ = max(fmaxZ, g_GBuffer.getPoint(x, y).z);
		}
	float fDepth = fmaxZ - fminZ; // scale factor

	norm_mutex.lock();
	for(int x = 0; x < g_width; x++)
		for(int y = 0; y < g_height; y++)
		{
			float depth = (g_GBuffer.getPoint(x,y).z - fminZ) / fDepth;
			g_normals.setPixel(x, y, Color(depth, depth, depth));
		}
	norm_mutex.unlock();
}

void createScene()
{
	//// some new things
	/////fov = 3.14159 / 3.2
	Vector camPos	= Vector(0, 0, 1);
	Vector lookAt	= Vector(0, 0, 0);
	Vector upDir	= Vector(0, 1, 0);
	g_camera.setPositionAndLookAt(3.14159 / 3.2,
		camPos, lookAt, upDir, g_width, g_height);

	// add some spheres

	Sphere *sph1 = new Sphere(1.0f, Vector(0.75, -1, -4.75 - 1), Color::yellow);
	Sphere *sph2 = new Sphere(0.75f, Vector(-0.75, -1.25, -3.5 - 1), Color::blue);
	Sphere *sph3 = new Sphere(0.4f, Vector(0.5, -1.6, -3.5 - 1), Color::green);
	g_objects.push_back(sph1);
	g_objects.push_back(sph2);
	g_objects.push_back(sph3);

	// add some boxes
	Box *box1 = new Box(Vector(-2, -2, -6), Vector(2, 2, 6.1),
				Color::white * 0.8f);	
	g_objects.push_back(box1);
	Triangle *t1 = new Triangle(Vector(-2, -2, -5), Vector(2, -2, 1.1), Vector(2, -2, -5), Color::blue);
	//g_objects.push_back(t1);

	// add some lights
	AmbientLight *alight = new AmbientLight(0.9f * Color::yellow);
	DirectionalLight *dirlight = new DirectionalLight(Color::white, -Vector(0.2f, 1.0f, 0.6f));
	
	g_lights.push_back(alight);
	//g_lights.push_back(dirlight);
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

// rejection sampling
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
	int kernel_size = 256; // sample count
	float fDistance;
	Vector normal;
	Color col; //received color, dummy

	// intersection?
	if(intersectObjects(r, fDistance, normal, col))
	{
		// perform AO
		static const float epsilon = 0.0001f;

		// determine intersection position
		Vector point = r.origin + (r.direction * fDistance);

		// calc a basis for the local hemisphere
		Vector tangent;
		Vector binormal;
		
		// construct tangent and binormal
		// if x, y != 0 choose t = (-n2 n1 0)^T
		// else t = (0 -n3 n2)^T
		if(abs(normal.x) > epsilon || abs(normal.y) > epsilon)
		{
			tangent = Vector(-normal.y, normal.x, 0.0f);
		}
		else
		{
			tangent = Vector(0.0f, -normal.z, normal.y);
		}
		tangent.normalize();

		// use cross product to determine binormal
		binormal = tangent.crossproduct(normal);

		// normalize
		tangent.normalize();
		binormal.normalize();

		// some assertions
		assert(normal * tangent < epsilon);
		assert(normal * binormal < epsilon);
		assert(tangent * binormal < epsilon);

		// shoot random rays
		Ray kernel_ray(point, Vector()); // init with position

		// dummies
		float kernel_fdist;
		Vector kernel_normal;
		Color kernel_color;

		// now perform AO
		float occlusion_factor = 0.0;
		for(int i = 0; i < kernel_size; i++)
		{
			// construct local random ray, through basis
			// note that we use a hemisphere, therefore the random value for the normal is in [0, 1]
			Vector v = random(-1.0f, 1.0f) * tangent + random(-1.0f, 1.0f) * binormal + random(0.0, 1.0f) * normal;
			v.normalize();
				
			kernel_ray.direction = v;

			// intersect with scene
			if(intersectObjects(kernel_ray, kernel_fdist, kernel_normal, kernel_color))
			{
				assert(kernel_fdist >= 0.0);
				
				// range check
				if(kernel_fdist < 0.40f && kernel_fdist > 0.0001f)occlusion_factor += 1.0; // simply add(maybe later account light better)
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
	// raytrace Image
	Raytrace();
	
	// perform AmbientOcclusion pass
	AmbientOcclusionPass();

	
	// composite images...

	// blur
	invao_mutex.lock();
	g_invao.copyFrom(g_aopass);
	g_invao.blur();
	g_invao.invert();
	// uncomment to get darker
	//g_invao.normalize();
	invao_mutex.unlock();

	// composite
	final_mutex.lock();
	g_final.copyFrom(g_image);
	g_final.multiply(g_invao);
	final_mutex.unlock();
}

int main(int argc, char * argv[])
{
	// set up images
	g_image.create(g_width, g_height);
	g_normals.create(g_width, g_height);
	g_aopass.create(g_width, g_height);
	g_invao.create(g_width, g_height);
	g_final.create(g_width, g_height);

	// set up GBuffer
	g_GBuffer = GBuffer(g_width, g_height);

	// start mode is 0
	mode = 0;

	srand((unsigned)time(0));

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
		invao_mutex.lock();
		final_mutex.lock();

		Draw();
		
		// swap buffers
		glfwSwapBuffers();

		final_mutex.unlock();
		invao_mutex.unlock();
		norm_mutex.unlock();
		ao_mutex.unlock();
		img_mutex.unlock();		

		// switch render modes...
		if(glfwGetKey(GLFW_KEY_F1)){mode = 0; img_mutex.lock(); g_image.forceupdate(); img_mutex.unlock();}
		if(glfwGetKey(GLFW_KEY_F3)){mode = 1; ao_mutex.lock(); g_aopass.forceupdate(); ao_mutex.unlock();}
		if(glfwGetKey(GLFW_KEY_F2)){mode = 2; norm_mutex.lock(); g_normals.forceupdate(); norm_mutex.unlock();}
		if(glfwGetKey(GLFW_KEY_F4)){mode = 3; invao_mutex.lock(); g_invao.forceupdate(); invao_mutex.unlock();}
		if(glfwGetKey(GLFW_KEY_F5)){mode = 4; final_mutex.lock(); g_final.forceupdate(); final_mutex.unlock();}

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