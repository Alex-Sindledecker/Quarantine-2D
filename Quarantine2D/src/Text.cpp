#include "pch.h"
#include "Text.h"
#include "Logger.h"
#include "Game.h"

namespace q2d
{

	unsigned int Font::instances = 0;
	FT_Library Font::ft;

	Font::Font(std::string src, unsigned int size)
	{
		init(src, size);
	}

	Font::~Font()
	{
		FT_Done_Face(face);
		if (--instances == 0)
			FT_Done_FreeType(ft);
	}

	void Font::init(std::string src, unsigned int size)
	{
		glyphs.resize(128);
		if (++instances == 1)
		{
			if (FT_Init_FreeType(&ft))
				Q2D_LOGC("ERROR! Failed to init FreeType");
		}

		const char* s = src.c_str();
		if (FT_New_Face(ft, s, 0, &face))
			Q2D_LOGC("ERROR! Failed to load font " + src);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_TEXTURE_RES, FONT_TEXTURE_RES, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		setSize(size);
	}

	void Font::setSize(unsigned int size)
	{
		GLint xOffset = 1, yOffset = 1;
		this->size = size;

		FT_Set_Pixel_Sizes(face, 0, size);
		glBindTexture(GL_TEXTURE_2D, id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (GLubyte c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				Q2D_LOGC("ERROR! Failed to load char " << c);
				continue;
			}

			glTexSubImage2D(
				GL_TEXTURE_2D, 0,
				xOffset, yOffset, face->glyph->bitmap.width, face->glyph->bitmap.rows,
				GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glyphs[c].advance = face->glyph->advance.x >> 6;
			glyphs[c].bearingX = face->glyph->bitmap_left;
			glyphs[c].bearingY = face->glyph->bitmap_top;
			glyphs[c].uvWidth = face->glyph->bitmap.width / (double)FONT_TEXTURE_RES;
			glyphs[c].uvHeight = face->glyph->bitmap.rows / (double)FONT_TEXTURE_RES;
			glyphs[c].uvx = xOffset / (double)FONT_TEXTURE_RES;
			glyphs[c].uvy = yOffset / (double)FONT_TEXTURE_RES;
			glyphs[c].width = face->glyph->bitmap.width;
			glyphs[c].height = face->glyph->bitmap.rows;

			xOffset += face->glyph->bitmap.width + 3;
			if (xOffset + FONT_TEXTURE_RES / 12 > FONT_TEXTURE_RES)
			{
				xOffset = 1;
				yOffset += size + 3;
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	unsigned int Font::getSize() const
	{
		return size;
	}

	Glyph Font::getGlyph(unsigned char c) const
	{
		return glyphs[c];
	}

	Text::Text(std::string text, float x, float y, Font* font)
	{
		init(text, x, y, font);
	}

	Text::~Text()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void Text::init(std::string text, float x, float y, Font* font)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		shader.init("res/shaders/textVertex.glsl", "res/shaders/textFragment.glsl");
		this->x = x;
		this->y = y;
		setFont(font);
		setText(text);
	}

	void Text::setPos(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void Text::setText(std::string text)
	{
		this->text = text;
		build();
	}

	void Text::setFont(Font* font)
	{
		this->font = font;
	}

	void Text::setColor(Color color)
	{
		this->color = color;
	}

	void Text::render()
	{
		shader.use();
		shader.setUniformMatrix4("projection", glm::ortho(0.f, Game::winWidth, Game::winHeight, 0.f) * glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0)));
		shader.setUniformVector4("color", glm::vec4(color.r, color.g, color.b, color.a));
		font->bind();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, text.length() * 6);
	}

	void Text::build()
	{
		maxWidth = 0;
		maxHeight = 0;
		std::vector<Vector4> vertices(text.length() * 6);
		float vx = 0, vy = font->getSize();
		long long vIndex = 0;
		for (unsigned char c : text)
		{
			if (c == (unsigned char)'\n')
			{
				vy += font->getSize();
				vx = x;
				continue;
			}
			Glyph g = font->getGlyph(c);

			vertices[vIndex] = Vector4(vx + g.bearingX + g.width, vy - g.bearingY, g.uvx + g.uvWidth, g.uvy);
			vertices[vIndex + 1] = Vector4(vx + g.bearingX, vy - g.bearingY, g.uvx, g.uvy);
			vertices[vIndex + 2] = Vector4(vx + g.bearingX, vy - g.bearingY + g.height, g.uvx, g.uvy + g.uvHeight);
			vertices[vIndex + 3] = Vector4( vx + g.bearingX, vy - g.bearingY + g.height, g.uvx, g.uvy + g.uvHeight);
			vertices[vIndex + 4] = Vector4(vx + g.bearingX + g.width, vy - g.bearingY + g.height, g.uvx + g.uvWidth, g.uvy + g.uvHeight);
			vertices[vIndex + 5] = Vector4(vx + g.bearingX + g.width, vy - g.bearingY, g.uvx + g.uvWidth, g.uvy);

			vx += g.advance;
			maxWidth = vx > maxWidth ? vx : maxWidth;
			maxHeight = vy;
			vIndex += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		if (vertices.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Vector4 Text::getRect() const
	{
		return Vector4(x, y, maxWidth, maxHeight);
	}

	std::string Text::getText() const
	{
		return text;
	}

	bool Text::mouseOver() const
	{
		return Game::mouseX > x && Game::mouseX < x + maxWidth && Game::mouseY > y && Game::mouseY < y + maxHeight;
	}

}