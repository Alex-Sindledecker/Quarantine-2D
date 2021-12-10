#include "PauseMenu.h"
#include <SpriteRenderer.h>

void PauseMenu::init()
{
	font.init("res/font/ethnocen.ttf", 24);
	resumeButton.init("Resume", 0, 0, &font);
	quitButton.init("Quit Game", 0, 0, &font);
}

void PauseMenu::update(float dt)
{
	if (resumeButton.mouseOver())
		resumeButton.setColor(q2d::Color(0, 1, 0));
	else
		resumeButton.setColor(q2d::Color(1, 0, 0));

	if (quitButton.mouseOver())
		quitButton.setColor(q2d::Color(0, 1, 0));
	else
		quitButton.setColor(q2d::Color(1, 0, 0));

	resumeButton.setPos(viewportWidth / 2.f - resumeButton.getRect().z / 2.f, viewportHeight * 0.4f - resumeButton.getRect().w / 2.f);
	quitButton.setPos(viewportWidth / 2.f - quitButton.getRect().z / 2.f, viewportHeight * 0.6f - quitButton.getRect().w / 2.f);
}

void PauseMenu::draw()
{
	q2d::SpriteRenderer::renderQuad(viewportWidth / 2.f - 120.f, viewportHeight / 2.f - 130.f, 240.f, 260.f, 0.f, q2d::Color(0.14, 0.25, 0.43f, 0.5f));
	resumeButton.render();
	quitButton.render();
}
