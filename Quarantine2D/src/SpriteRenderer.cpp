#include "pch.h"
#include "Game.h"
#include "SpriteRenderer.h"

namespace q2d
{

	GLuint SpriteRenderer::quadVao = 0;
	Shader SpriteRenderer::quadShader;
	Shader SpriteRenderer::spriteShader;

	//Initialized the sprite renderer
	void SpriteRenderer::init()
	{
		quadShader.init("res/shaders/quadVertex.glsl", "res/shaders/quadFragment.glsl");
		spriteShader.init("res/shaders/spriteVertex.glsl", "res/shaders/spriteFragment.glsl");

		GLuint quadVbo;
		glGenVertexArrays(1, &quadVao);
		glGenBuffers(1, &quadVbo);
		glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBindVertexArray(quadVao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Renders a sprite
	void SpriteRenderer::renderSprite(Sprite sprite, float texX, float texY, float texW, float texH)
	{
		glm::mat4 proj = glm::ortho(0.f, Game::winWidth, Game::winHeight, 0.f);
		glm::mat4 model = makeModelMatrix(sprite.x, sprite.y, sprite.width, sprite.height, sprite.theta);

		spriteShader.use();
		spriteShader.setUniformMatrix4("projection", proj);
		spriteShader.setUniformMatrix4("model", model);
		spriteShader.setUniformVector4("texRect", glm::vec4(texX, texY, texW, texH));
		sprite.texture.bind();

		glBindVertexArray(quadVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//Renders a colored quad
	void SpriteRenderer::renderQuad(float x, float y, float width, float height, float theta, Color color)
	{
		glm::mat4 proj = glm::ortho(0.f, Game::winWidth, Game::winHeight, 0.f);
		glm::mat4 model = makeModelMatrix(x, y, width, height, theta);

		quadShader.use();
		quadShader.setUniformMatrix4("projection", proj);
		quadShader.setUniformMatrix4("model", model);
		quadShader.setUniformVector4("color", glm::vec4(color.r, color.g, color.b, color.a));

		glBindVertexArray(quadVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void SpriteRenderer::destroy()
	{
		glDeleteVertexArrays(1, &quadVao);
	}

	SpriteBatch::SpriteBatch(Texture texture, unsigned int batchSize)
	{
		init(texture, batchSize);
	}

	SpriteBatch::~SpriteBatch()
	{
	}

	void SpriteBatch::init(Texture texture, unsigned int batchSize)
	{
		this->texture = texture;
		this->batchSize = batchSize;
		quadIndex = 0;
		quads.resize(batchSize);
		quadIndex = batchSize - 1;

		shader.init("res/shaders/batchVertex.glsl", "res/shaders/batchFragment.glsl");

		if (vao == 0)
		{
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4) * 6 * batchSize, NULL, GL_DYNAMIC_DRAW);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SpriteBatch::addQuad(BatchQuad quad)
	{
		quads[quadIndex] = quad;
		quads[quadIndex].active = true;
		quadIndex = --quadIndex % batchSize;
	}

	void SpriteBatch::emptyBatch()
	{
		for (BatchQuad& quad : quads)
		{
			quad.active = false;
		}
		quadIndex = batchSize - 1;
	}

	void SpriteBatch::build()
	{
		std::vector<Vector4> vertices(batchSize * 6);
		unsigned int count = 0;
		for (BatchQuad& quad : quads)
		{
			if (quad.active)
			{
				glm::mat4 model = makeModelMatrix(quad.x, quad.y, quad.width, quad.height, quad.theta);

				glm::vec4 v0 = glm::vec4(glm::vec2(model * glm::vec4(quadVertices[0], quadVertices[1], 0, 1)), quadVertices[0], quadVertices[1]);
				glm::vec4 v1 = glm::vec4(glm::vec2(model * glm::vec4(quadVertices[2], quadVertices[3], 0, 1)), quadVertices[2], quadVertices[3]);
				glm::vec4 v2 = glm::vec4(glm::vec2(model * glm::vec4(quadVertices[4], quadVertices[5], 0, 1)), quadVertices[4], quadVertices[5]);
				glm::vec4 v3 = glm::vec4(glm::vec2(model * glm::vec4(quadVertices[6], quadVertices[7], 0, 1)), quadVertices[6], quadVertices[7]);
				glm::vec4 v4 = glm::vec4(glm::vec2(model * glm::vec4(quadVertices[8], quadVertices[9], 0, 1)), quadVertices[8], quadVertices[9]);
				glm::vec4 v5 = glm::vec4(glm::vec2(model * glm::vec4(quadVertices[10], quadVertices[11], 0, 1)), quadVertices[10], quadVertices[11]);

				vertices[count] = Vector4(v0.x, v0.y, quad.uv[0] + quad.uv[2], quad.uv[1]);
				vertices[count + 1] = Vector4(v1.x, v1.y, quad.uv[0], quad.uv[1]);
				vertices[count + 2] = Vector4(v2.x, v2.y, quad.uv[0], quad.uv[1] + quad.uv[3]);
				vertices[count + 3] = Vector4(v3.x, v3.y, quad.uv[0], quad.uv[1] + quad.uv[3]);
				vertices[count + 4] = Vector4(v4.x, v4.y, quad.uv[0] + quad.uv[2], quad.uv[1] + quad.uv[3]);
				vertices[count + 5] = Vector4(v5.x, v5.y, quad.uv[0] + quad.uv[2], quad.uv[1]);

				count += 6;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Vector4), &vertices[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SpriteBatch::render()
	{
		glm::mat4 proj = glm::ortho(0.f, Game::winWidth, Game::winHeight, 0.f);
		shader.use();
		shader.setUniformMatrix4("projection", proj);
		texture.bind();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, batchSize * 6);
	}

	BatchQuad& SpriteBatch::operator[](const unsigned int& index)
	{
		return quads[index];
	}

}