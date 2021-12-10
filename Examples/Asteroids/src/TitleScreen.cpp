#include "TitleScreen.h"

void TitleScreen::init()
{
	font.init("res/font/ethnocen.ttf", 48);
	playButton.init("Play Game!", 0, 0, &font);
	quitButton.init("Quit Game", 0, 0, &font);
}

void TitleScreen::update(float dt)
{
	if (playButton.mouseOver())
		playButton.setColor(q2d::Color(0, 1, 0));
	else
		playButton.setColor(q2d::Color(1, 0, 0));

	if (quitButton.mouseOver())
		quitButton.setColor(q2d::Color(0, 1, 0));
	else
		quitButton.setColor(q2d::Color(1, 0, 0));

	playButton.setPos(viewportWidth / 2.f - playButton.getRect().z / 2.f, viewportHeight * 0.25f - playButton.getRect().w / 2.f);
	quitButton.setPos(viewportWidth / 2.f - quitButton.getRect().z / 2.f, viewportHeight * 0.75f - quitButton.getRect().w / 2.f);
}

void TitleScreen::draw()
{
	playButton.render();
	quitButton.render();
}
