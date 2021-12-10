#include "GameScreen.h"

#include <random>
#include <time.h>
#include <algorithm>

void GameScreen::init()
{
	srand(time(NULL));

	particles.init();

	asteroidSpriteSheet.init("res/tex/AsteroidsTilemap.png", false, false);
	playerTexture.init("res/tex/Player.png", false, true);
	playerBoostTexture.init("res/tex/PlayerBoost.png", false, true);

	player.x = viewportWidth / 2.f;
	player.y = viewportHeight / 2.f;
	player.w = 42;
	player.h = 42;
	player.a = 0;
	player.texRect = q2d::Vector4(0, 0, 1, 1);

	gameFont.init("res/font/ethnocen.ttf", 32);
	gameOverText.init("GAME OVER!", viewportWidth / 2.f, viewportHeight / 2.f, &gameFont);
	gameOverText.setPos(viewportWidth / 2.f - gameOverText.getRect().z / 2.f, viewportHeight / 2.f - gameOverText.getRect().w / 2.f);
	gameOverText.setText("");
	gameOverText.setColor(q2d::Color(1, 0, 0));
	scoreText.init("Score: 0", 20, 20.f, &gameFont);
	scoreText.setColor(q2d::Color(1, 0, 0));
}

void GameScreen::update(float dt)
{
	static float timeSum = 0;
	timeSum += dt;
	if (lives > 0)
	{
		updatePlayer(dt);
		updateBullets(dt);
		updateAsteroids(dt);
	}
	else
	{
		if (timeSum > 0.3f)
		{
			static const char text[] = { 'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R', '!' };
			static int index = 0;

			if (index < 10)
			{
				gameOverText.setText(gameOverText.getText() + text[index]);
				index++;
			}
			timeSum = 0;
		}
	}
	particles.update(dt);
}

void GameScreen::draw()
{
	for (GameObject bullet : bullets)
		drawGameObject(bullet, q2d::Color(1, 1, 1));
	for (GameObject asteroid : asteroids)
		drawGameObject(asteroid, asteroidSpriteSheet);
	drawGameObject(player, playerTexture);
	if (drawBoost)
		drawGameObject(player, playerBoostTexture);

	particles.render();

	GameObject life = player;
	life.x = 20;
	life.y = 68;
	life.w /= 1.5f;
	life.h /= 1.5f;
	life.a = 0;
	for (int i = 0; i < lives; i++)
	{
		drawGameObject(life, playerTexture);
		life.x += life.w + 20.f;
	}

	if (lives == 0)
		gameOverText.render();
	scoreText.render();
}

void GameScreen::updatePlayer(float dt)
{
	static float timeSum = 0.f;
	timeSum += dt;

	const float rotationSpeed = 4;
	const float m = 1000.f;

	if (gamePtr->getKeyboardButtonState(q2d::KeyboardButton::RIGHT) == q2d::ButtonState::PRESS)
	{
		player.a += dt * rotationSpeed;
	}
	if (gamePtr->getKeyboardButtonState(q2d::KeyboardButton::LEFT) == q2d::ButtonState::PRESS)
	{
		player.a -= dt * rotationSpeed;
	}
	if (gamePtr->getKeyboardButtonState(q2d::KeyboardButton::UP) == q2d::ButtonState::PRESS)
	{
		player.vx += sin(player.a) * m * dt;
		player.vy += -cos(player.a) * m * dt;

		if (timeSum > 0.05f)
		{
			drawBoost ^= 1;
			timeSum = 0.f;
		}
	}
	else
	{
		drawBoost = false;
	}

	player.vx *= exp(-dt / 0.7f);
	player.vy *= exp(-dt / 0.7f);

	const float velocityThreshold = 0.1f;
	if (abs(player.vx) < velocityThreshold)
		player.vx = 0;
	if (abs(player.vy) < velocityThreshold)
		player.vy = 0;

	player.x += player.vx * dt;
	player.y += player.vy * dt;

	wrapObject(player, playerTexture);
}

void GameScreen::updateBullets(float dt)
{
	static bool spaceKeyWentUp = false;
	if (gamePtr->getKeyboardButtonState(q2d::KeyboardButton::SPACE) == q2d::ButtonState::RELEASE)
		spaceKeyWentUp = true;

	if (spaceKeyWentUp && gamePtr->getKeyboardButtonState(q2d::KeyboardButton::SPACE) == q2d::ButtonState::PRESS)
	{
		spaceKeyWentUp = false;
		const float v = 1200.f;
		GameObject bullet;
		bullet.x = player.x + player.w / 2.f - 2.5f;
		bullet.y = player.y + player.h / 2.f - 2.5f;
		bullet.w = 5.f;
		bullet.h = 5.f;
		bullet.a = player.a;
		bullet.vx = v * sin(player.a);
		bullet.vy = v * -cos(player.a);
		bullet.av = 0;
		bullets.push_back(bullet);
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i].x += bullets[i].vx * dt;
		bullets[i].y += bullets[i].vy * dt;
		if (bullets[i].x + bullets[i].w < 0 || bullets[i].x > viewportWidth || bullets[i].y + bullets[i].h < 0 || bullets[i].y > viewportHeight)
		{
			bullets.erase(bullets.begin() + i);
		}
	}
}

void GameScreen::updateAsteroids(float dt)
{
	if (asteroids.size() == 0)
	{
		for (int i = 0; i < 5; i++)
		{
			GameObject asteroid = spawnBigAsteroid(rand() % (int)viewportWidth, rand() % (int)viewportHeight);
			asteroid.vx = rand() % int(maxAsteroidVelocity * 2) - maxAsteroidVelocity;
			asteroid.vy = rand() % int(maxAsteroidVelocity * 2) - maxAsteroidVelocity;
			asteroid.av = rand() % 70;

			asteroids.push_back(asteroid);
		}
	}

	for (int i = 0; i < asteroids.size(); i++)
	{
		asteroids[i].x += asteroids[i].vx * dt;
		asteroids[i].y += asteroids[i].vy * dt;
		asteroids[i].a += 3.14f / 180.f * asteroids[i].av * dt;
		wrapObject(asteroids[i], asteroidSpriteSheet);

		if (checkCollision(player, asteroids[i]))
		{
			killAsteroid(asteroids[i], dt);
			explodeGameObject(player, 5.f, dt);
			player.x = viewportWidth / 2.f;
			player.y = viewportHeight / 2.f;

			asteroids.erase(asteroids.begin() + i);
			lives--;
			i--;
			continue;
		}
		for (int n = 0; n < bullets.size(); n++)
		{
			if (checkCollision(bullets[n], asteroids[i]))
			{
				killAsteroid(asteroids[i], dt);
				asteroids.erase(asteroids.begin() + i);
				bullets.erase(bullets.begin() + n);
				i--;
				n--;
				break;
			}
		}
	}
}

void GameScreen::drawGameObject(const GameObject& gameObject, const q2d::Texture& texture)
{
	static q2d::Sprite sprite;
	sprite.texture = texture;
	sprite.width = gameObject.w;
	sprite.height = gameObject.h;
	sprite.x = gameObject.x;
	sprite.y = gameObject.y;
	sprite.theta = gameObject.a;
	q2d::SpriteRenderer::renderSprite(sprite, gameObject.texRect.x, gameObject.texRect.y, gameObject.texRect.z, gameObject.texRect.w);
}

void GameScreen::drawGameObject(const GameObject& gameObject, const q2d::Color& color)
{
	q2d::SpriteRenderer::renderQuad(gameObject.x, gameObject.y, gameObject.w, gameObject.h, gameObject.a, color);
}

void GameScreen::wrapObject(GameObject& gameObject, const q2d::Texture& texture)
{
	GameObject temp = gameObject;

	if (viewportWidth - (gameObject.x + gameObject.w) < 1.f)
	{
		temp.x = -(viewportWidth - gameObject.x);
		drawGameObject(temp, texture);
		temp.x = gameObject.x;
	}
	else if (gameObject.x < 0 && gameObject.x > -gameObject.w)
	{
		temp.x = viewportWidth + gameObject.x;
		drawGameObject(temp, texture);
		temp.x = gameObject.x;
	}
	if (viewportHeight - (gameObject.y + gameObject.h) < 1.f)
	{
		temp.y = -(viewportHeight - gameObject.y);
		drawGameObject(temp, texture);
		temp.y = gameObject.y;
	}
	else if (gameObject.y < 0 && gameObject.y > -gameObject.h)
	{
		temp.y = viewportHeight + gameObject.y;
		drawGameObject(temp, texture);
		temp.y = gameObject.y;
	}

	if (gameObject.x > viewportWidth)
		gameObject.x = 0;
	if (gameObject.y > viewportHeight)
		gameObject.y = 0;
	if (gameObject.x < -gameObject.w)
		gameObject.x = viewportWidth - gameObject.w;
	if (gameObject.y < -gameObject.h)
		gameObject.y = viewportHeight - gameObject.h;
}

void GameScreen::explodeGameObject(const GameObject& object, float size, float dt)
{
	q2d::ParticleSettings particleSettings;
	particleSettings.startColor = q2d::Color(1, 0, 0);
	particleSettings.endColor = q2d::Color(1, 0, 1, 0);
	particleSettings.x = object.x;
	particleSettings.y = object.y;
	particleSettings.w = size;
	particleSettings.h = size;
	particleSettings.lifetime = pow(size / 6.f, 1.5f);
	for (int i = 0; i < 360; i += 5)
	{
		particleSettings.x = object.x + rand() % int(object.w);
		particleSettings.y = object.y + rand() % int(object.h);
		particleSettings.vx = cos(3.14f / 180.f * i) * (800.f / size) + rand() % 30 - 15.f;
		particleSettings.vy = sin(3.14f / 180.f * i) * (800.f / size) + rand() % 30 - 15.f;
		particleSettings.av = rand() % 100;
		particles.emit(particleSettings);
	}
}

void GameScreen::killAsteroid(const GameObject& asteroid, float dt)
{
	if (asteroid.w == 128)
	{
		score += 128;
		for (int i = 0; i < 2; i++)
		{
			GameObject newAsteroid = spawnMediumAsteroid(asteroid.x + rand() % 64 - 32, asteroid.y + rand() % 64 - 32);
			newAsteroid.vx = rand() % int(maxAsteroidVelocity * 2) - maxAsteroidVelocity;
			newAsteroid.vy = rand() % int(maxAsteroidVelocity * 2) - maxAsteroidVelocity;
			newAsteroid.av = rand() % 70;

			asteroids.push_back(newAsteroid);
		}
		explodeGameObject(asteroid, 6.f, dt);
	}
	else if (asteroid.w == 64)
	{
		score += 64;
		for (int i = 0; i < 2; i++)
		{
			GameObject newAsteroid = spawnSmallAsteroid(asteroid.x + rand() % 64 - 32, asteroid.y + rand() % 64 - 32);
			newAsteroid.vx = rand() % int(maxAsteroidVelocity * 2) - maxAsteroidVelocity;
			newAsteroid.vy = rand() % int(maxAsteroidVelocity * 2) - maxAsteroidVelocity;
			newAsteroid.av = rand() % 70;

			asteroids.push_back(newAsteroid);
		}
		explodeGameObject(asteroid, 5.f, dt);
	}
	else
	{
		score += 32;
		explodeGameObject(asteroid, 4.f, dt);
	}
	scoreText.setText("Score: " + std::to_string(score));
}

bool GameScreen::checkCollision(const GameObject& object1, const GameObject& object2)
{
	if (object2.x > object1.x && object2.x < object1.x + object1.w && object2.y > object1.y && object2.y < object1.y + object1.h)
		return true;
	if (object1.x > object2.x && object1.x < object2.x + object2.w && object1.y > object2.y && object1.y < object2.y + object2.h)
		return true;
	return false;
}

GameObject GameScreen::spawnBigAsteroid(float x, float y)
{
	static q2d::Vector4 texCoords[3] = { 
		q2d::Vector4(0, 0, 0.5, 0.5),
		q2d::Vector4(0.5, 0, 0.5, 0.5),
		q2d::Vector4(0, 0.5, 0.5, 0.5) };

	GameObject asteroid;
	asteroid.x = x;
	asteroid.y = y;
	asteroid.w = 128;
	asteroid.h = 128;
	asteroid.a = 3.14f / 180.f * float(rand() % 360);
	asteroid.texRect = texCoords[rand() % 3];

	return asteroid;
}

GameObject GameScreen::spawnMediumAsteroid(float x, float y)
{
	static q2d::Vector4 texCoords[3] = {
		q2d::Vector4(0.5, 0.5, 0.25, 0.25),
		q2d::Vector4(0.75, 0.5, 0.25, 0.25),
		q2d::Vector4(0.5, 0.75, 0.25, 0.25) };

	GameObject asteroid;
	asteroid.x = x;
	asteroid.y = y;
	asteroid.w = 64;
	asteroid.h = 64;
	asteroid.a = 3.14f / 180.f * float(rand() % 360);
	asteroid.texRect = texCoords[rand() % 3];

	return asteroid;
}

GameObject GameScreen::spawnSmallAsteroid(float x, float y)
{
	static q2d::Vector4 texCoords[3] = {
		q2d::Vector4(0.75, 0.75, 0.125, 0.125),
		q2d::Vector4(0.875, 0.75, 0.125, 0.125),
		q2d::Vector4(0.75, 0.875, 0.125, 0.125) };

	GameObject asteroid;
	asteroid.x = x;
	asteroid.y = y;
	asteroid.w = 32;
	asteroid.h = 32;
	asteroid.a = 3.14f / 180.f * float(rand() % 360);
	asteroid.texRect = texCoords[rand() % 3];

	return asteroid;
}
