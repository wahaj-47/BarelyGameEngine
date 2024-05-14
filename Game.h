#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

struct WindowConfig { int width, height, fullscreen; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig	{ int SR, CR, OR, OG, OB, OT, VMin, VMax, L, SI; float SMin, SMax; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow	m_window;			// Game window
	EntityManager		m_entities;		// Vector of entities to mantain
	sf::Font			m_font;			// Game font
	sf::Text			m_text;			// Score text
	WindowConfig		m_windowConfig;
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	bool				m_paused = false;
	bool				m_running = true;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);
	void setPaused(bool paused);

	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

	int randomInRange(int max);
	int randomInRange(int min, int max);
	sf::Color randomColor();

public:

	Game(const std::string& config);

	void run();

};

