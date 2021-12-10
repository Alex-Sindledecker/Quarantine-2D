#pragma once

#include "Config.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace q2d
{

	class Q2D_RENDERING_API Shader
	{
	public:
		Shader();
		~Shader();

		void init(std::string vertexShader, std::string fragmentShader);
		void destroy();
		void use() const;
		void setUniformMatrix4(const char* location, glm::mat4 matrix);
		void setUniformVector4(const char* location, glm::vec4 vector);
		void setUniformVector3(const char* location, glm::vec3 vector);
		void setUniformFloat(const char* location, float value);
		void setUniformInt(const char* location, int value);

	private:
		struct ShaderSource
		{
			std::string vertexShader, fragmentShader;
		};

		static ShaderSource readShader(std::string vertexShader, std::string fragmentShader);

		GLuint id;
	};

}