/**
 * @file    fire.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "pgr.h"

class CFire
{
public:
	void initializeFire();
	void drawFire(glm::mat4 viewMatrix, float time, glm::mat4& projectionMatrix);
	
	glm::vec3 fire_position;
	float fire_size;

	GLuint fireTexture;
	GLuint fireVertexBufferObject;
	GLuint fireElementBufferObject;
	GLuint fireVertexArrayObject;

	struct FireShaderProgram {
		GLuint program;
		// vertex attributes locations
		GLint posLocation;
		GLint texCoordLocation;
		// uniforms locations
		GLint PVMmatrixLocation;
		GLint VmatrixLocation;
		GLint timeLocation;
		GLint texSamplerLocation;
		GLint frameDurationLocation;
		GLfloat animationSize = 1;
	}fireShaderProgram;
};