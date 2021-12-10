#include "pch.h"
#include "Game.h"
#include "Logger.h"
#include "SpriteRenderer.h"
#include "PostProcessor.h"

namespace q2d
{

	float Game::winWidth = 0;
	float Game::winHeight = 0;
	double Game::mouseX = 0;
	double Game::mouseY = 0;

	Game::Game(float winWidth, float winHeight, const char* title, bool runPostProcessor)
		: activeState(nullptr)
	{
		this->runPostProcessor = runPostProcessor;
		this->winWidth = winWidth;
		this->winHeight = winHeight;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		windowPtr = glfwCreateWindow(winWidth, winHeight, title, NULL, NULL);
		if (windowPtr == NULL)
		{
			Q2D_LOGC("ERROR! GLFW Window Creation Failed!");
			glfwTerminate();
		}
		glfwMakeContextCurrent(windowPtr);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Q2D_LOGC("ERROR! OpenGL context creation failed!");
			glfwTerminate();
		}
		glfwSetWindowUserPointer(windowPtr, this);
		glfwSetWindowSizeCallback(windowPtr, screenResizeCallback);
		glfwSetKeyCallback(windowPtr, keyToggleCallback);
		glfwSetMouseButtonCallback(windowPtr, mouseButtonToggleCallback);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.3f, 0.21f, 0.72f, 1.f);
		glViewport(0, 0, winWidth, winHeight);

		if (runPostProcessor)
			PostProcessor::init(winWidth, winHeight);
		SpriteRenderer::init();
	}

	Game::~Game()
	{
		glfwTerminate();
		SpriteRenderer::destroy();
		if (runPostProcessor)
			PostProcessor::destroy();
	}

	bool Game::closeRequested() const
	{
		return glfwWindowShouldClose(windowPtr);
	}

	void Game::run()
	{
		float dt = glfwGetTime();
		glfwSetTime(0);
		glfwGetCursorPos(windowPtr, &mouseX, &mouseY);

		if (runPostProcessor)
			PostProcessor::activate();
		glClear(GL_COLOR_BUFFER_BIT);

		for (StateCollection state : states)
		{
			if (state.isTriggered(state.arg1, state.arg2) == true)
				stateQueue.push(state.statePtr);
		}

		while (stateQueue.size() > 0)
		{
			activeState = stateQueue.front();

			if (activeState != nullptr)
			{
				activeState->_setViewport(winWidth, winHeight);
				activeState->_setMousePos(mouseX, mouseY);
				activeState->_setGamePtr(this);
				activeState->update(dt);
				activeState->draw();
			}

			stateQueue.pop();
		}

		if (runPostProcessor)
		{
			PostProcessor::deactivate();
			PostProcessor::renderResults();
		}

		glfwSwapBuffers(windowPtr);
		glfwPollEvents();
	}

	void Game::addState(IGameState* state, TriggerFunction when, void* arg1, void* arg2)
	{
		StateCollection collection;
		collection.statePtr = state;
		collection.isTriggered = when;
		collection.arg1 = arg1;
		collection.arg2 = arg2;
		state->_setViewport(winWidth, winHeight);
		state->init();
		states.push_back(collection);
	}

	void Game::setClearColor(Color color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Game::quit()
	{
		glfwSetWindowShouldClose(windowPtr, true);
	}

	GLFWwindow* Game::getWindowPtr() const
	{
		return windowPtr;
	}

	ButtonState Game::getMouseButtonState(MouseButton button)
	{
		return (ButtonState)glfwGetMouseButton(windowPtr, (int)button);
	}

	ButtonState Game::getKeyboardButtonState(KeyboardButton button)
	{
		return (ButtonState)glfwGetKey(windowPtr, (int)button);
	}

	void Game::screenResizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Game::winWidth = width;
		Game::winHeight = height;
		PostProcessor::resize(width, height);
	}

	void Game::keyToggleCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

	}

	void Game::mouseButtonToggleCallback(GLFWwindow* window, int button, int action, int mods)
	{

	}

	void IGameState::_setViewport(float width, float height)
	{
		viewportWidth = width;
		viewportHeight = height;
	}

	void IGameState::_setMousePos(float x, float y)
	{
		mouseX = x;
		mouseY = y;
	}

	void IGameState::_setGamePtr(Game* game)
	{
		gamePtr = game;
	}

}