// Object Space Ambient Occlusion Demo Project
// (c) 2013 by L.Spiegelberg
// This code may be redistributed or modified for any learning or teaching purposes
// but not for any commercial uses
// to gain further license informations please contact me via
// spiegelb (at) in.tum.de


#ifndef IMAGE_HEADER_
#define IMAGE_HEADER_

#include "Color.h"

class Image
{
private:
	/// a color map to hold the image data
	Color *data; 

	/// width/height of image
	int		width;
	int		height;

	/// OpenGL Texture ID
	GLuint	id;

	/// does texture need to be updated?
	bool	modified;

	/// internal update function
	void	update()
	{
		// select texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, id);


		// use for the texture 32 Bit format
		unsigned long *buffer = new unsigned long[width * height];

		// set buffer to internal pointer
		for(int i = 0; i < width * height; i++)
		{
			buffer[i] = data[i]; // casting is done automatically

			// OpenGL stores colors in ABGR order, so it is necessary to flip bits
			buffer[i] = ARGBToABGR(buffer[i]);
		}

		// set tetxure
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
		glDisable(GL_TEXTURE_2D);

		delete [] buffer;
	}

public:

	Image():data(NULL), width(0), height(0), id(-1), modified(false)		{}

	Image(const int _width, const int _height)
	{
		modified = false;
		id		= -1;
		width	= _width;
		height	= _height;
		data	= new Color[width * height];
	}

	~Image()
	{
		// delete GL textures
		if(id >= 0)glDeleteTextures(1, &id);

		if(data)delete [] data;
		data = NULL;
	}


	/// create image manually
	inline void create(const int _width, const int _height)
	{
		modified = true;
		width	= _width;
		height	= _height;

		// remove data
		if(data)delete [] data;

		data	= new Color[width * height];
	}

	/// set pixel
	inline void	setPixel(const int x, const int y, const Color& c)
	{
		assert(x >= 0 && x < width);
		assert(y >= 0 && y < height);

		// texture update necessary
		modified = true;

		data[x + y * width] = c;
	}

	/// get pixel
	inline Color getPixel(const int x, const int y)
	{
		assert(x >= 0 && x < width);
		assert(y >= 0 && y < height);

		return data[x + y * width];
	}

	/// creates or updates internal OpenGL texture representation
	GLuint getTexture() 
	{
		// initialized?
		if(id < 0)
		{
			// generate new texture
			glGenTextures(1, &id);
			modified = true;
		}

		// perform update if necessary
		if(modified)update();
		modified = false;

		return id;
	}

	/// force a update for the gl Texture
	void	forceupdate()	{update();}

	/// get width/height
	inline int getWidth() const {return width;}
	inline int getHeight() const {return height;}

	/// blur image
	void	blur()
	{
		// construct a kernel
		int kernel_sizex = 9;
		int kernel_sizey = 9;

		// kernel size has to be odd
		assert(kernel_sizex & 0x1);
		assert(kernel_sizey & 0x1);

		float	*kernel = new float[kernel_sizex * kernel_sizey];

		for(int i = 0; i < kernel_sizex; i++)
			for(int j = 0; j < kernel_sizey; j++)
			{
				kernel[i + j * kernel_sizey] = 1.0f / (float)(kernel_sizex * kernel_sizey);
			}

		// go through image and apply kernel
		
		// temp array
		Color *temp = new Color[width * height];
		
		// zero colors
		for(int i = 0;  i < width * height; i++)temp[i] = Color(0.0f, 0.0f, 0.0f); 

		for(int x = 0;  x < width; x++)
			for(int y = 0; y < height; y++)
			{
				// apply kernel (with periodic boundaries)
				for(int i = 0; i < kernel_sizex; i++)
					for(int j = 0; j < kernel_sizey; j++)
					{
						int xindex = x + i - kernel_sizex / 2;
						int yindex = y + j - kernel_sizey / 2;

						// apply periodic conditions
						if(xindex >= width)xindex -= width;
						if(yindex >= height)yindex -= height;
						if(xindex < 0)xindex += width;
						if(yindex < 0)yindex += height;


						assert(xindex >= 0 && xindex < width);
						assert(yindex >= 0 && yindex < height);
						
						// add kernel
						temp[x + y * width] = temp[x + y * width]   +   kernel[i + j * kernel_sizex] * data[xindex + yindex * width];
					}
			}


		// set pointers
		Color *delpointer = data;
		data = temp;
		delete [] delpointer;


		delete [] kernel;

		update();
	}

	/// copy image from another
	void	copyFrom(const Image& img)
	{
		// delete data
		if(data)delete [] data;
		width = img.width;
		height = img.height;

		data = new Color[width * height];

		// copy
		for(int i = 0; i < width * height; i++)data[i] = img.data[i];

		update();
	}

	/// inverse
	inline void invert()
	{
		for(int i = 0; i < width * height; i++)data[i] = Color(1.0f, 1.0f, 1.0f) - data[i];

		update();
	}

	/// multiply with image
	void	multiply(const Image& img)
	{
		//check dimensions
		assert(width == img.width);
		assert(height == img.height);

		for(int i = 0; i < width * height; i++)data[i] = data[i] * img.data[i];

		update();
	}

	/// normalize image(stretch values to 0.0 - 1.0)
	/// except for alpha
	void normalize()
	{
		using namespace std;
		
		Color cmin = Color(99999.9f,99999.9f,99999.9f);
		Color cmax = Color(-99999.9f,-99999.9f,-99999.9f);

		for(int i = 0; i < width * height; i++)
		{
			cmin.r = min(cmin.r, data[i].r);
			cmin.g = min(cmin.g, data[i].g);
			cmin.b = min(cmin.b, data[i].b);

			cmax.r = max(cmax.r, data[i].r);
			cmax.g = max(cmax.g, data[i].g);
			cmax.b = max(cmax.b, data[i].b);
		}

		Color cscale = cmax - cmin;

		for(int i = 0; i < width * height; i++)
		{
			data[i] = data[i] * cscale - cmin;
		}

		update();
	}
};


#endif