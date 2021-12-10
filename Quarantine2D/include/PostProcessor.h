#pragma once

#include "Texture.h"
#include "Shader.h"

#include <unordered_map>

namespace q2d
{

	enum class Q2D_RENDERING_API ParamType
	{
		FLOAT,
		INT,
		MATRIX4,
		VECTOR4,
		VECTOR3
	};

	struct Q2D_RENDERING_API ShaderParameter
	{
		ParamType type;
		union
		{
			glm::mat4 mat;
			glm::vec4 v4;
			glm::vec3 v3;
			float f;
			int i;
		} value;
	};

	class Q2D_RENDERING_API PostProcessor
	{
	public:
		static void init(unsigned int winWidth, unsigned int winHeight);
		static void resize(unsigned int winWidth, unsigned int winHeight);
		static void setShaderParameter(std::string name, ShaderParameter param);
		static void activate();
		static void deactivate();
		static void renderResults();
		static void destroy();

	private:
		PostProcessor() {}

		static Texture screenTexture;
		static Shader postEffectsShader;
		static GLuint fbo;
		static GLuint vao;
		static std::unordered_map<std::string, ShaderParameter> shaderParams;
	};

}