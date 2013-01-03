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

	/// get width/height
	inline int getWidth() const {return width;}
	inline int getHeight() const {return height;}
};


#endif