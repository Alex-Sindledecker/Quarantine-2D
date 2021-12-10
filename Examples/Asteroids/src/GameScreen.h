#pragma once

#include <Game.h>
#include <SpriteRenderer.h>
#include <ParticleEmitter.h>
#include <Text.h>

struct GameObject
{
	float x, y, w, h, a;
	float vx, vy, av;
	q2d::Vector4 texRect;
};

constexpr float maxAsteroidVelocity = 100.f;

class GameScreen : public q2d::IGameState
{
public:
	void init() override;
	void update(float dt) override;
	void draw() override;

	void updatePlayer(float dt);
	void updateBullets(float dt);
	void updateAsteroids(float dt);
	void drawGameObject(const GameObject& gameObject, const q2d::Texture& texture);
	void drawGameObject(const GameObject& gameObject, const q2d::Color& color);
	void wrapObject(GameObject& gameObject, const q2d::Texture& texture);
	void explodeGameObject(const GameObject& object, float size, float dt);
	void killAsteroid(const GameObject& asteroid, float dt);
	bool checkCollision(const GameObject& object1, const GameObject& object2);
	GameObject spawnBigAsteroid(float x, float y);
	GameObject spawnMediumAsteroid(float x, float y);
	GameObject spawnSmallAsteroid(float x, float y);

private:
	q2d::Texture asteroidSpriteSheet;
	q2d::Texture playerTexture;
	q2d::Texture playerBoostTexture;
	q2d::ParticleEmitter particles;
	q2d::Font gameFont;
	q2d::Text gameOverText;
	q2d::Text scoreText;

	std::vector<GameObject> asteroids;
	std::vector<GameObject> bullets;
	GameObject player;
	bool drawBoost = false;

	float lives = 3;
	int score = 0;
};