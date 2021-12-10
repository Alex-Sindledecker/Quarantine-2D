#include "pch.h"
#include "ParticleEmitter.h"
#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

namespace q2d
{

	Shader ParticleEmitter::shader;
	GLuint ParticleEmitter::vao = 0, ParticleEmitter::vbo;
	int ParticleEmitter::instances = 0;

	ParticleEmitter::ParticleEmitter()
	{
		instances++;
		init();
	}

	ParticleEmitter::~ParticleEmitter()
	{
		instances--;
		if (instances == 0)
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
		}
		glDeleteBuffers(2, instancedBuffers);
	}

	void ParticleEmitter::init()
	{
		this->particleIndex = MAX_PARTICLES - 1;
		particles.resize(MAX_PARTICLES);

		if (vao == 0)
		{
			shader.init("res/shaders/particleVertex.glsl", "res/shaders/particleFragment.glsl");

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
			glBindVertexArray(vao);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		}

		glGenBuffers(2, instancedBuffers);
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffers[0]);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffers[1]);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Color), NULL, GL_DYNAMIC_DRAW);

		glBindVertexArray(vao);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)0);
		glVertexAttribDivisor(1, 1);

		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffers[0]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
		glVertexAttribDivisor(2, 1);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
		glVertexAttribDivisor(3, 1);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(8 * sizeof(float)));
		glVertexAttribDivisor(4, 1);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(12 * sizeof(float)));
		glVertexAttribDivisor(5, 1);
		glBindVertexArray(0);
	}

	void ParticleEmitter::emit(ParticleSettings emitProperties)
	{
		particles[particleIndex].active = true;
		particles[particleIndex].x = emitProperties.x;
		particles[particleIndex].y = emitProperties.y;
		particles[particleIndex].a = rand() % 360;
		particles[particleIndex].vx = emitProperties.vx;
		particles[particleIndex].vy = emitProperties.vy;
		particles[particleIndex].av = emitProperties.av;
		particles[particleIndex].startColor = emitProperties.startColor;
		particles[particleIndex].endColor = emitProperties.endColor;
		particles[particleIndex].life = emitProperties.lifetime;
		particles[particleIndex].lifetime = emitProperties.lifetime;
		particles[particleIndex].w = emitProperties.w;
		particles[particleIndex].h = emitProperties.h;

		particleIndex = --particleIndex % MAX_PARTICLES;
	}

	void ParticleEmitter::update(float dt)
	{
		for (Particle& particle : particles)
		{
			if (particle.active)
			{
				if (particle.life == 0)
				{
					particle.active = false;
					continue;
				}
				particle.life -= dt;
				particle.x += particle.vx * dt;
				particle.y += particle.vy * dt;
				particle.a += particle.av * dt;
				if (particle.life < 0)
					particle.life = 0;
			}
		}
	}

	void ParticleEmitter::render()
	{
		int index = 0;
		std::vector<glm::mat4> models(MAX_PARTICLES);
		std::vector<Color> colors(MAX_PARTICLES);
		for (Particle& particle : particles)
		{
			if (particle.active)
			{
				models[index] = makeModelMatrix(particle.x, particle.y, particle.w, particle.h, particle.a);
				glm::vec4 startColor = glm::vec4(particle.startColor.r, particle.startColor.g, particle.startColor.b, particle.startColor.a);
				glm::vec4 endColor = glm::vec4(particle.endColor.r, particle.endColor.g, particle.endColor.b, particle.endColor.a);
				glm::vec4 intColor = glm::lerp(endColor, startColor, particle.life / particle.lifetime);
				colors[index] = Color(intColor.x, intColor.y, intColor.z, intColor.w);
				index++;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffers[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(glm::mat4), &models[0]);
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffers[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(Color), &colors[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		shader.use();
		shader.setUniformMatrix4("projection", glm::ortho(0.f, Game::winWidth, Game::winHeight, 0.f));

		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, index);
	}

}