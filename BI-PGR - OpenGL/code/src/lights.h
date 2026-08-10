/**
 * @file    lights.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "pgr.h"
#include "constants.h"

struct Sun 
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3  direction;
	float speed;
};

struct PointLight 
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3  position;
	float maxDistance;
};

struct SpotLight 
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3  position;      
	glm::vec3  direction; 
	float cosCutOff; // cosine of the spotlight's half angle
	float exponent;  // distribution of the light energy within the reflector's cone (center->cone's edge)
};

class CLights 
{
public:
	Sun sun;
	SpotLight spotLight;
	PointLight pointLight;
	glm::vec3 globalAmbientLight;

	void initializeLights();
	void update(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, const GameState& game_state);
};
