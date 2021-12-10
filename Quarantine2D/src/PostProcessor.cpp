#include "pch.h"
#include "PostProcessor.h"
#include "Logger.h"
#include "SpriteRenderer.h"
#include "RenderingTools.h"

namespace q2d
{
	Shader PostProcessor::postEffectsShader;
	Texture PostProcessor::screenTexture;
	GLuint PostProcessor::fbo, PostProcessor::vao;
	std::unordered_map<std::string, ShaderParameter> PostProcessor::shaderParams;

	void PostProcessor::init(unsigned int winWidth, unsigned int winHeight)
	{
		postEffectsShader.init("res/shaders/postProcVertex.glsl", "res/shaders/postProcFragment.glsl");
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		screenTexture.initEmpty(winWidth, winHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.getId(), 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Q2D_LOGC("ERROR! Bad framebuffer!");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLuint vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void PostProcessor::resize(unsigned int winWidth, unsigned int winHeight)
	{
		screenTexture.initEmpty(winWidth, winHeight);
	}

	void PostProcessor::activate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void PostProcessor::deactivate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PostProcessor::renderResults()
	{
		postEffectsShader.use();
		for (auto param : shaderParams)
		{
			switch (param.second.type)
			{
			case ParamType::MATRIX4:
				postEffectsShader.setUniformMatrix4(param.first.c_str(), param.second.value.mat);
				break;
			case ParamType::VECTOR4:
				postEffectsShader.setUniformVector4(param.first.c_str(), param.second.value.v4);
				break;
			case ParamType::VECTOR3:
				postEffectsShader.setUniformVector3(param.first.c_str(), param.second.value.v3);
				break;
			case ParamType::FLOAT:
				postEffectsShader.setUniformFloat(param.first.c_str(), param.second.value.f);
				break;
			case ParamType::INT:
				postEffectsShader.setUniformInt(param.first.c_str(), param.second.value.i);
			}
		}
		screenTexture.bind();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void PostProcessor::destroy()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteFramebuffers(1, &fbo);
	}

	void PostProcessor::setShaderParameter(std::string name, ShaderParameter param)
	{
		shaderParams[name] = param;
	}

}