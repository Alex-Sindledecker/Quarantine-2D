#include "pch.h"
#include "Texture.h"
#include "tools/stb_image.h"

namespace q2d
{

	Texture::Texture()
	{
		id = 0;
	}

	Texture::~Texture()
	{
	}

	void Texture::init(const char* src, bool mipmaps, bool smooth)
	{
		if (id == 0)
			glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (smooth)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		int w, h, channels;
		unsigned char* pixels = stbi_load(src, &w, &h, &channels, 0);
		if (pixels)
		{
			GLint pixelFormat;
			switch (channels)
			{
			case 3:
				pixelFormat = GL_RGB;
				break;
			case 4:
				pixelFormat = GL_RGBA;
				break;
			default:
				pixelFormat = GL_RED;
				break;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, w, h, 0, pixelFormat, GL_UNSIGNED_BYTE, pixels);
			if (mipmaps)
				glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(pixels);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::initEmpty(unsigned int width, unsigned int height)
	{
		if (id == 0)
			glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::destroy()
	{
		glDeleteTextures(1, &id);
	}

	void Texture::bind() const
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	GLuint Texture::getId() const
	{
		return id;
	}

}