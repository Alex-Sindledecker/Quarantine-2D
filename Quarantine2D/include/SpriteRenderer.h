#pragma once

#include "Texture.h"
#include "Shader.h"
#include "RenderingTools.h"

#include <vector>

namespace q2d
{

	struct Q2D_RENDERING_API Sprite
	{
		float x, y, width, height, theta;
		Texture texture;
	};

	struct Q2D_RENDERING_API BatchQuad
	{
		float x, y, width, height, theta;
		Vector4 uv = {0, 0, 1, 1};
		bool active = false;
	};

	class Q2D_RENDERING_API SpriteRenderer
	{
	public:
		static void init();
		static void renderSprite(Sprite sprite, float texX = 0, float texY = 0, float texW = 1, float texH = 1);
		static void renderQuad(float x, float y, float width, float height, float theta, Color color);
		static void destroy();

	private:
		SpriteRenderer() {}

		static GLuint quadVao;
		static Shader quadShader;
		static Shader spriteShader;
	};

	class Q2D_RENDERING_API SpriteBatch
	{
	public:
		SpriteBatch(Texture texture, unsigned int batchSize);
		SpriteBatch() {}
		~SpriteBatch();

		void init(Texture texture, unsigned int batchSize);
		void addQuad(BatchQuad quad);
		void emptyBatch();
		void build();
		void render();
		BatchQuad& operator[](const unsigned int& index);

	private:
		GLuint vao = 0, vbo = 0;
		Texture texture;
		Shader shader;
		std::vector<BatchQuad> quads;
		unsigned int quadIndex, batchSize;
	};

}