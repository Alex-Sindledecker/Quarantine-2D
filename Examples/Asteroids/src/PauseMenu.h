#pragma once

#include <Game.h>
#include <Text.h>

class PauseMenu : public q2d::IGameState
{
public:
	void init() override;
	void update(float dt) override;
	void draw() override;

	q2d::Text resumeButton;
	q2d::Text quitButton;

private:
	q2d::Font font;
};