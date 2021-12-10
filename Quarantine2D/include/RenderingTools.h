#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace q2d
{

	struct Color
	{
		float r, g, b, a;
		Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
		Color() : r(0), g(0), b(0), a(0) {}
	};

	struct Vector4
	{
		float x, y, z, w;
		Vector4() : x(0), y(0), z(0), w(0) {}
		Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		float& operator[](const unsigned int& index)
		{
			switch (index)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				return w;
			}
		}
	};

	static GLfloat quadVertices[] = {
		1.f, 1.f,
		0.f, 1.f,
		0.f, 0.f,

		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f
	};

	static glm::mat4 makeModelMatrix(float x, float y, float w, float h, float theta)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(x + w / 2.f, y + h / 2.f, 0.f));
		model = glm::rotate(model, theta, glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(-w / 2.f, -h / 2.f, 1.f));
		return glm::scale(model, glm::vec3(w, h, 1));
	}

}