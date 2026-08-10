/**
 * @file    smoke.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "smoke.h"
#include "constants.h"

void CSmoke::initializeSmoke() 
{
	smokeTexture = pgr::createTexture(SMOKE_TEXTURE_PATH, true);

	glGenVertexArrays(1, &smokeVertexArrayObject);
	glBindVertexArray(smokeVertexArrayObject);

	glGenBuffers(1, &smokeVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, smokeVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smokeVertexData), smokeVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(smokeShaderProgram.positionLocation);
	glEnableVertexAttribArray(smokeShaderProgram.texCoordLocation);
	glVertexAttribPointer(smokeShaderProgram.positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glVertexAttribPointer(smokeShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	CHECK_GL_ERROR();
}

void CSmoke::drawSmoke(glm::mat4 viewMatrix, glm::mat4& projectionMatrix, GameState& game_state)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(smokeShaderProgram.program);

	glm::mat4 billboardRotationMatrix = glm::mat4(
		viewMatrix[0],
		viewMatrix[1],
		viewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);
	//glm::mat4 projectionMatrix = glm::perspective(45.0f, (GLfloat)(glutGet(GLUT_WINDOW_WIDTH)) / (GLfloat)(glutGet(GLUT_WINDOW_HEIGHT)), 0.1f, 1000.0f);
	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(smoke_position.x, smoke_position.y, smoke_position.z));
	matrix = glm::scale(matrix, glm::vec3(smoke_size));

	// make billboard to face camera
	matrix = matrix * billboardRotationMatrix;
	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;

	glUniformMatrix4fv(smokeShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
	glUniform1f(smokeShaderProgram.timeLocation, game_state.smoke_current_time - game_state.smoke_start_time);
	glUniform1i(smokeShaderProgram.texSamplerLocation, 0);

	glBindVertexArray(smokeVertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, smokeTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, smokeNumQuadVertices);

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	CHECK_GL_ERROR();
}