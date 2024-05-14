#include "Game.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include "windows.h"

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// Read in config file
	std::string temp;
	std::ifstream fin(path);

	while (fin >> temp)
	{
		if (temp == "Window")
		{
			fin >> m_windowConfig.width >> m_windowConfig.height >> m_windowConfig.fullscreen;
		}
		if (temp == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S
				>> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
				>> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT
				>> m_playerConfig.V;
		}
		if (temp == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR
				>> m_enemyConfig.SMin >> m_enemyConfig.SMax
				>> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT
				>> m_enemyConfig.VMin >> m_enemyConfig.VMax
				>> m_enemyConfig.L >> m_enemyConfig.SI;
				
		}
		if (temp == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
				>> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
				>> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT
				>> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	// Set up default windows parameters
	m_window.create(sf::VideoMode(m_windowConfig.width, m_windowConfig.height), "Barely Game Engine", m_windowConfig.fullscreen == 1 ? sf::Style::Fullscreen : sf::Style::Resize);
	m_window.setFramerateLimit(60);

	spawnPlayer();
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();
		
		if (!m_paused) 
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
		}
		sLifespan();
		sRender();

		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::spawnPlayer()
{

	// Add all properties of the player using the config values

	// We create all entities by calling EntityManager.addEntity(tag)
	// This returns std::shared_ptr<Entity>, so we are using 'auto' to save typing
	auto entity = m_entities.addEntity("player");

	// TODO: Spawn player at the center of the screen
	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

	// The entity shape will have a radius of 32, 8 sides, dark grey fill, and red outline of thickness 4
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, 
											  m_playerConfig.V, 
											  sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), 
											  sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), 
											  m_playerConfig.OT);

	// Add an input component to handle inputs
	entity->cInput = std::make_shared<CInput>();

	// Add collision 
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// This entity is our player, so we will set this to be our Game's player variable
	// This goes against the EntityManager paradigm, but we use the player so much that this is worth it
	m_player = entity;
}

void Game::spawnEnemy()
{
	// Spawn enemy with properties from m_enemyConfig variable and completely within the bounds of the screen

	auto entity = m_entities.addEntity("enemy");

	float halfShapeRadius = m_enemyConfig.SR / 2.0f;

	// TODO: Spawn player at the center of the screen
	float mx = randomInRange(halfShapeRadius, m_window.getSize().x - halfShapeRadius);
	float my = randomInRange(halfShapeRadius, m_window.getSize().y - halfShapeRadius);

	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(randomInRange(m_enemyConfig.SMin, m_enemyConfig.SMax), randomInRange(m_enemyConfig.SMin, m_enemyConfig.SMax)), 0.0f);

	// The entity shape will have a radius of 32, 8 sides, dark grey fill, and red outline of thickness 4
	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randomInRange(m_enemyConfig.VMin, m_enemyConfig.VMax), randomColor(), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

	// Add collision 
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	// Record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	int v = entity->cShape->circle.getPointCount();
	float angle = (360.0f / v) * (M_PI / 180.0f);

	for (int i = 0; i < v; i++)
	{
		auto e = m_entities.addEntity("smallEnemy");
		e->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(2 * cos(i * angle), 2 * sin(i * angle)), 0.0f);
		e->cShape = std::make_shared<CShape>(entity->cShape->circle.getRadius() / 2, v, entity->cShape->circle.getFillColor(), entity->cShape->circle.getOutlineColor(), entity->cShape->circle.getOutlineThickness());
		e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
	}

	entity->destroy();

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	// TODO: Implement spawning of a bullet which travels towards a target
	//		 - Bullet speed is given as a scalar
	//		 - Set the velocity using formula in notes
	auto bullet = m_entities.addEntity("bullet");
	
	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, ((target - entity->cTransform->pos) / target.dist(entity->cTransform->pos)) * m_bulletConfig.S, 0.0f);
	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	if (m_entities.getEntities("enemy").size() < 2)
	{
		return;
	}

	Vec2 lastEnemyPos = entity->cTransform->pos;

	for (auto& e : m_entities.getEntities("enemy"))
	{
		e->cTransform->velocity = { 0,0 };

		Vec2 currentPos = e->cTransform->pos;

		for (float p=0; p<=1; p += 0.016f)
		{
			auto special = m_entities.addEntity("special");
			special->cTransform = std::make_shared<CTransform>(currentPos, Vec2(0,0), 0.0f);
			special->cShape = std::make_shared<CShape>(m_bulletConfig.SR/2, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
			special->cLifespan = std::make_shared<CLifespan>(120);

			currentPos = lastEnemyPos.lerp(e->cTransform->pos, p);
		}

		lastEnemyPos = e->cTransform->pos;

		spawnSmallEnemies(e);

	}

}

void Game::sEnemySpawner()
{
	// TODO: Use (m_currentFrame - m_lastEnemySpawnTime) to determine how long it
	//		 has been since last enemy spawned
	if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::sMovement()
{
	// TODO: Implement all entity movement in this function
	//		 you should read the player->cInput component to determine if the player is moving

	for (auto& e : m_entities.getEntities())
	{
		if (e->cInput)
		{
			e->cTransform->velocity = { 0,0 };

			if (e->cInput->up && e->cTransform->pos.y - e->cCollision->radius > 0)
			{
				e->cTransform->velocity.y -= 10;
			}
			if (e->cInput->down && e->cTransform->pos.y + e->cCollision->radius < m_window.getSize().y)
			{
				e->cTransform->velocity.y += 10;
			}

			if (e->cInput->left && e->cTransform->pos.x - e->cCollision->radius > 0)
			{
				e->cTransform->velocity.x -= 10;
			}
			if (e->cInput->right && e->cTransform->pos.x + e->cCollision->radius < m_window.getSize().x)
			{
				e->cTransform->velocity.x += 10;
			}
		}

		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}

}

void Game::sCollision()
{
	// TODO: Implement all collisions between entities
	//		 Use collision radius instead of the shape radius

	for (auto& e : m_entities.getEntities())
	{
		if (e->cCollision)
		{
			if (e->cTransform->pos.x - e->cCollision->radius <= 0 || e->cTransform->pos.x + e->cCollision->radius >= m_window.getSize().x)
			{
				e->cTransform->velocity.x *= -1;
			}
			if (e->cTransform->pos.y - e->cCollision->radius <= 0 || e->cTransform->pos.y + e->cCollision->radius >= m_window.getSize().y)
			{
				e->cTransform->velocity.y *= -1;
			}
		}
	}

	for (auto& e : m_entities.getEntities("enemy"))
	{
		if (e->cTransform->pos.distSqrd(m_player->cTransform->pos) < (e->cCollision->radius + m_player->cCollision->radius) * (e->cCollision->radius + m_player->cCollision->radius))
		{
			e->destroy();
			float mx = m_window.getSize().x / 2.0f;
			float my = m_window.getSize().y / 2.0f;
			m_player->cTransform->pos = { mx, my };
		}
		for (auto& b : m_entities.getEntities("bullet"))
		{
			if (b->cTransform->pos.distSqrd(e->cTransform->pos) < (b->cCollision->radius + e->cCollision->radius) * (b->cCollision->radius + e->cCollision->radius))
			{
				spawnSmallEnemies(e);
				b->destroy();
			}
		}
	}
}

void Game::sRender()
{
	// TODO: Change code below to draw all of the entities
	//		 Sample drawing of the player entity
	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		// Set the position of the shape based on the transform of the entity
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// Set the rotation of the shape based on the entity transform angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		// Draw the entites to the window
		m_window.draw(e->cShape->circle);
	}

	m_window.display();


}

void Game::sUserInput()
{
	// TOOD: Handle user input here
	//		 note you should only be setting player's input component variables here
	//		 you should not implement player's movement logic here
	//		 the movement system will read the variables you set here

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// This event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// This event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W key pressed" << std::endl;
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				std::cout << "S key pressed" << std::endl;
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				std::cout << "A key pressed" << std::endl;
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				std::cout << "D key pressed" << std::endl;
				m_player->cInput->right = true;
				break;
			}
		}

		// This event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W key released" << std::endl;
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				std::cout << "S key released" << std::endl;
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				std::cout << "A key released" << std::endl;
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				std::cout << "D key released" << std::endl;
				m_player->cInput->right = false;
				break;
			}
		}

		// This event is triggered when a key is released
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "Left mouse button pressed" << std::endl;
				// Spawn bullet here
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "Right mouse button pressed" << std::endl;
				// Spawn special weapon here
				spawnSpecialWeapon(m_player);
			}
		}
	}
}

void Game::sLifespan()
{
	// TODO: Implement all lifespan functionality

	// for all entities
	//		if entity has no lifespan component, skip it
	//		if entity has > 0 remaining lifespan, subtract 1
	//		if it has lifespan and is alive
	//			scale its alpha channel properly
	//		if it has lifespan and its time is up
	//			destroy the entity

	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan)
		{
			if (e->cLifespan->remaining > 0)
			{
				e->cLifespan->remaining -= 1;

				sf::Color fillColor = e->cShape->circle.getFillColor();
				fillColor.a = 255.0 * e->cLifespan->remaining / e->cLifespan->total;

				sf::Color outlineColor = e->cShape->circle.getOutlineColor();
				outlineColor.a = 255.0 * e->cLifespan->remaining / e->cLifespan->total;

				e->cShape->circle.setFillColor(fillColor);
				e->cShape->circle.setOutlineColor(outlineColor);
			}
			if (e->cLifespan->remaining <= 0)
			{
				e->destroy();
			}
		}
	}
}

int Game::randomInRange(int max)
{
	int range = max - 1;
	return (std::rand() % range);
}

int Game::randomInRange(int min, int max)
{
	int range = max - min + 1;
	return (std::rand() % range) + min;
}

sf::Color Game::randomColor()
{
	return sf::Color(randomInRange(255), randomInRange(255), randomInRange(255));
}
