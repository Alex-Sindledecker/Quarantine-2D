#include "pch.h"
#include "Shader.h"
#include "Logger.h"

namespace q2d
{

	Shader::Shader()
	{
		id = 0;
	}

	Shader::~Shader()
	{
	}

	void Shader::init(std::string vertexSrc, std::string fragmentSrc)
	{
		ShaderSource src = readShader(vertexSrc, fragmentSrc);
		const char* vertexContent = src.vertexShader.c_str();
		const char* fragmentContent = src.fragmentShader.c_str();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexShader, 1, &vertexContent, NULL);
		glShaderSource(fragmentShader, 1, &fragmentContent, NULL);
		glCompileShader(vertexShader);
		glCompileShader(fragmentShader);
#ifdef _DEBUG
		GLint success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			Q2D_LOGC("ERROR! There was a compile error in the vertex shader!");
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			Q2D_LOGC(infoLog);
		}
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			Q2D_LOGC("ERROR! There was a compile error in the fragment shader!");
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			Q2D_LOGC(infoLog);
		}
#endif

		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);

		glDetachShader(id, vertexShader);
		glDetachShader(id, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Shader::destroy()
	{
		glDeleteProgram(id);
	}

	void Shader::use() const
	{
		glUseProgram(id);
	}

	void Shader::setUniformMatrix4(const char* location, glm::mat4 matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(id, location), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::setUniformVector4(const char* location, glm::vec4 vector)
	{
		glUniform4fv(glGetUniformLocation(id, location), 1, &vector[0]);
	}

	void Shader::setUniformVector3(const char* location, glm::vec3 vector)
	{
		glUniform3fv(glGetUniformLocation(id, location), 1, &vector[0]);
	}

	void Shader::setUniformFloat(const char* location, float value)
	{
		glUniform1f(glGetUniformLocation(id, location), value);
	}

	void Shader::setUniformInt(const char* location, int value)
	{
		glUniform1i(glGetUniformLocation(id, location), value);
	}

	Shader::ShaderSource Shader::readShader(std::string vertexShader, std::string fragmentShader)
	{
		ShaderSource src;

		std::ifstream reader;

		reader.open(vertexShader);
		if (reader.is_open())
		{
			std::stringstream ss;
			ss << reader.rdbuf();
			src.vertexShader = ss.str();
			reader.close();
		}

		reader.open(fragmentShader);
		if (reader.is_open())
		{
			std::stringstream ss;
			ss << reader.rdbuf();
			src.fragmentShader = ss.str();
			reader.close();
		}

		return src;
	}

}