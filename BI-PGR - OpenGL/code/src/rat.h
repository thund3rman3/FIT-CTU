/**
 * @file    rat.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "objects_render.h"
#include "constants.h"

struct CRat
{
	ObjectGeometry* rat_geo = nullptr;
	glm::vec3 init_pos;
	glm::vec3 direction;
	float     size;
	bool flipped = false;

	void initializeRat(ObjectGeometry** geometry, ShaderProgram* shaderProgram);
	void drawRat(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, ShaderProgram* shaderProgram);
};
