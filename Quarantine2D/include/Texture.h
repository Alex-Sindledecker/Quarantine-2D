#pragma once

#include "Config.h"

#include <glad/glad.h>

#include <string>

namespace q2d
{

	class Q2D_RENDERING_API Texture
	{
	public:
		Texture();
		virtual ~Texture();

		void init(const char* src, bool mipmaps, bool smooth);
		void initEmpty(unsigned int width, unsigned int height);
		void destroy();
		virtual void bind() const;
		GLuint getId() const;

	protected:
		GLuint id = 0;
	};

}