#pragma once

#include "Shader.h"
#include "Texture.h"
#include "RenderingTools.h"
#include <ft2Build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

namespace q2d
{

	struct Glyph
	{
		double bearingX, bearingY;
		double uvx, uvy, uvWidth, uvHeight;
		double width, height;
		GLuint advance;
	};

	constexpr GLsizei FONT_TEXTURE_RES = 512;

	class Q2D_RENDERING_API Font : public Texture
	{
	public:
		Font(std::string src, unsigned int size);
		Font() {}
		~Font();

		void init(std::string src, unsigned int size);
		void setSize(unsigned int size);
		unsigned int getSize() const;
		Glyph getGlyph(unsigned char c) const;

	private:
		static unsigned int instances;
		static FT_Library ft;
		unsigned int size;
		std::vector<Glyph> glyphs;
		FT_Face face;
	};

	class Q2D_RENDERING_API Text
	{
	public:
		Text(std::string text, float x, float y, Font* font);
		Text() {}
		~Text();

		void init(std::string text, float x, float y, Font* font);
		void setPos(float x, float y);
		void setText(std::string text);
		void setFont(Font* font);
		void setColor(Color color);
		void render();
		void build();
		bool mouseOver() const;
		Vector4 getRect() const;
		std::string getText() const;

	private:
		float x, y, maxWidth, maxHeight;
		GLuint vao, vbo;
		Shader shader;
		Color color;
		std::string text;
		Font* font;
	};

}