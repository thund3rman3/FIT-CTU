/**
 * @file    smoke.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "constants.h"
#include "pgr.h"

class CSmoke
{
public:
	void initializeSmoke();
	void drawSmoke(glm::mat4 viewMatrix, glm::mat4& projectionMatrix, GameState& game_state);

	GLuint smokeTexture;
	GLuint smokeVertexBufferObject;
	GLuint smokeElementBufferObject;
	GLuint smokeVertexArrayObject;

	glm::vec3 smoke_position;
	float smoke_size;

	struct SmokeShaderProgram {
		// shader id
		GLuint program;
		// uniforms locations
		GLint positionLocation;
		GLint timeLocation;
		GLint texSamplerLocation;
		GLint texCoordLocation;
		GLint PVMmatrixLocation;
	}smokeShaderProgram;
};