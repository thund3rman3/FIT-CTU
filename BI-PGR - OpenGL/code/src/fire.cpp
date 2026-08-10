/**
 * @file    fire.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "fire.h"
#include "constants.h"


void CFire::initializeFire()
{
	fireTexture = pgr::createTexture(FIRE_TEXTURE_PATH, true);

	glGenVertexArrays(1, &fireVertexArrayObject);
	glBindVertexArray(fireVertexArrayObject);

	glGenBuffers(1, &fireVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, fireVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertexData), fireVertexData, GL_STATIC_DRAW);


	glEnableVertexAttribArray(fireShaderProgram.posLocation);
	glVertexAttribPointer(fireShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(fireShaderProgram.texCoordLocation);
	glVertexAttribPointer(fireShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	CHECK_GL_ERROR();

}

void CFire::drawFire(glm::mat4 viewMatrix, float time, glm::mat4& projectionMatrix)
{
	fireShaderProgram.animationSize = fire_size;
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glUseProgram(fireShaderProgram.program);

	glm::mat4 billboardRotationMatrix = glm::mat4(
		viewMatrix[0],
		viewMatrix[1],
		viewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);
	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), fire_position);
	matrix = glm::scale(matrix, glm::vec3(fireShaderProgram.animationSize));

	// make billboard to face camera
	matrix = matrix * billboardRotationMatrix;
	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;

	glUniformMatrix4fv(fireShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
	glUniform1f(fireShaderProgram.timeLocation, time);

	CHECK_GL_ERROR();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fireTexture);
	glUniform1i(glGetUniformLocation(fireShaderProgram.program, "texSampler"), 0);


	glBindVertexArray(fireVertexArrayObject);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);
	CHECK_GL_ERROR();
}
