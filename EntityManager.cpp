#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
	// TODO: add entities from m_entitiesToAdd to the proper location(s)
	//		 - add them to vector of all entities
	//		 - add them to vector inside the map, with tag as a key
	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
	}

	m_entitiesToAdd.clear();

	// Remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	// Remove dead entities from each vector in the entity map
	// C+17 way to iterating through [key,value] pair in a map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// TODO: Remove all dead entities from the input vector
	//		 this is called by the update function
	// Hint: std::remove_if
	vec.erase(
		std::remove_if(vec.begin(), vec.end(), [](auto e) { return !e->isActive(); }),
		vec.end()
	);
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);
	m_entityMap[tag].push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	// TODO: This is incorrect, return correct vector from map
	return m_entityMap[tag];
}
