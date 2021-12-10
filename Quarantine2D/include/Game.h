#pragma once

#include "RenderingTools.h"
#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <queue>
#include <unordered_map>

namespace q2d
{

	typedef Q2D_RENDERING_API bool(*TriggerFunction)(void*, void*);

	enum class MouseButton
	{
		LEFT = GLFW_MOUSE_BUTTON_LEFT,
		RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
		MID = GLFW_MOUSE_BUTTON_MIDDLE
	};

	enum class KeyboardButton
	{
		LEFT = GLFW_KEY_LEFT, UP = GLFW_KEY_UP, RIGHT = GLFW_KEY_RIGHT, DOWN = GLFW_KEY_DOWN,
		SPACE = GLFW_KEY_SPACE, ESCAPE = GLFW_KEY_ESCAPE
	};

	enum class ButtonState
	{
		PRESS = GLFW_PRESS,
		RELEASE = GLFW_RELEASE
	};

	class Q2D_RENDERING_API Text;
	class Q2D_RENDERING_API SpriteRenderer;
	class Q2D_RENDERING_API SpriteBatch;
	class Q2D_RENDERING_API ParticleEmitter;
	class Q2D_RENDERING_API Game;

	class Q2D_RENDERING_API IGameState
	{
	public:
		virtual ~IGameState() {}
		virtual void init() = 0;
		virtual void update(float dt) = 0;
		virtual void draw() = 0;
		void _setViewport(float width, float height);
		void _setMousePos(float x, float y);
		void _setGamePtr(Game* game);

	protected:
		float viewportWidth, viewportHeight;
		float mouseX, mouseY;
		Game* gamePtr;
	};

	class Q2D_RENDERING_API Game
	{
	public:
		Game(float winWidth, float winHeight, const char* title, bool runPostProcessor = false);
		~Game();

		bool closeRequested() const;
		void run();
		void addState(IGameState* state, TriggerFunction when, void* arg1 = nullptr, void* arg2 = nullptr);
		void setClearColor(Color color);
		void quit();
		GLFWwindow* getWindowPtr() const;
		ButtonState getMouseButtonState(MouseButton button);
		ButtonState getKeyboardButtonState(KeyboardButton button);

		friend class Text;
		friend class SpriteRenderer;
		friend class SpriteBatch;
		friend class ParticleEmitter;

	private:
		struct Q2D_RENDERING_API StateCollection
		{
			TriggerFunction isTriggered;
			IGameState* statePtr;
			void* arg1;
			void* arg2;
		};

		static void screenResizeCallback(GLFWwindow* window, int width, int height);
		static void keyToggleCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseButtonToggleCallback(GLFWwindow* window, int button, int action, int mods);

		static float winWidth;
		static float winHeight;
		static double mouseX;
		static double mouseY;
		bool runPostProcessor;
		GLFWwindow* windowPtr;
		IGameState* activeState;
		std::vector<StateCollection> states;
		std::queue<IGameState*> stateQueue;
	};

}