#include "TitleScreen.h"
#include "PauseMenu.h"
#include "GameScreen.h"

enum class GameStates
{
	PLAY = 0,
	PAUSE = 1,
	TITLE,
	MENU
} activeState;

bool titleScreenActive(void* titleScreen, void* game)
{
	static bool mouseWentDown = false;

	if (activeState != GameStates::TITLE)
		return false;

	TitleScreen* screen = (TitleScreen*)titleScreen;
	q2d::Game* g = (q2d::Game*)game;

	q2d::ButtonState mouseButtonState = g->getMouseButtonState(q2d::MouseButton::LEFT);
	if (mouseButtonState == q2d::ButtonState::PRESS)
		mouseWentDown = true;

	if (screen->quitButton.mouseOver() && mouseWentDown && mouseButtonState == q2d::ButtonState::RELEASE)
	{
		g->quit();
	}
	else if (screen->playButton.mouseOver() && mouseWentDown && mouseButtonState == q2d::ButtonState::RELEASE)
	{
		activeState = GameStates::PLAY;
	}

	if (mouseButtonState == q2d::ButtonState::RELEASE)
		mouseWentDown = false;

	return true;
}

bool gameScreenActive(void*, void*)
{
	if (activeState == GameStates::PLAY)
		return true;
	return false;
}

bool pauseScreenActive(void* controlsScreen, void* game)
{
	static bool mouseWentDown = false;
	static bool escapeButtonWentDown = false;

	q2d::Game* g = (q2d::Game*)game;
	PauseMenu* screen = (PauseMenu*)controlsScreen;
	
	q2d::ButtonState mouseButtonState = g->getMouseButtonState(q2d::MouseButton::LEFT);
	if (mouseButtonState == q2d::ButtonState::PRESS)
		mouseWentDown = true;

	q2d::ButtonState escapeButtonState = g->getKeyboardButtonState(q2d::KeyboardButton::ESCAPE);
	if (escapeButtonState == q2d::ButtonState::PRESS)
		escapeButtonWentDown = true;

	if (activeState == GameStates::PLAY || activeState == GameStates::PAUSE)
	{
		if (escapeButtonWentDown && escapeButtonState == q2d::ButtonState::RELEASE)
		{
			activeState = (GameStates)((int)activeState ^ 1);
		}

		if (screen->resumeButton.mouseOver() && mouseWentDown && mouseButtonState == q2d::ButtonState::RELEASE)
		{
			activeState = GameStates::PLAY;
		}
		if (screen->quitButton.mouseOver() && mouseWentDown && mouseButtonState == q2d::ButtonState::RELEASE)
		{
			g->quit();
		}
	}

	if (mouseButtonState == q2d::ButtonState::RELEASE)
		mouseWentDown = false;
	if (escapeButtonState == q2d::ButtonState::RELEASE)
		escapeButtonWentDown = false;

	return activeState == GameStates::PAUSE;
}

int main()
{
	activeState = GameStates::TITLE;
	q2d::Game game(1280, 720, "Asteroids");

	TitleScreen titleScreen;
	GameScreen gameScreen;
	PauseMenu pauseScreen;
	game.addState(&titleScreen, titleScreenActive, &titleScreen, &game);
	game.addState(&gameScreen, gameScreenActive);
	game.addState(&pauseScreen, pauseScreenActive, &pauseScreen, &game);

	game.setClearColor(q2d::Color(0, 0, 0));
	while (!game.closeRequested())
	{
		game.run();
	}
}