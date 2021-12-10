#pragma once

#include "RenderingTools.h"
#include "Shader.h"

#include <vector>

namespace q2d
{

	struct Q2D_RENDERING_API ParticleSettings
	{
		double lifetime;
		float vx, vy, av;
		float x, y, w, h;
		Color startColor, endColor;
	};

	constexpr unsigned int MAX_PARTICLES = 1000;
	class Q2D_RENDERING_API ParticleEmitter
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

		void init();
		void emit(ParticleSettings emitProperties);
		void update(float dt);
		void render();

	private:
		struct Q2D_RENDERING_API Particle
		{
			float x = 0, y = 0, a = 0, w = 0, h = 0;
			float vx = 0, vy = 0, av = 0;
			float life = 0, lifetime = 0.000000001f;
			bool active = false;
			Color startColor, endColor;
		};

		std::vector<Particle> particles;
		GLuint instancedBuffers[2];
		int particleIndex;
		static GLuint vao;
		static GLuint vbo;
		static Shader shader;
		static int instances;
	};

}