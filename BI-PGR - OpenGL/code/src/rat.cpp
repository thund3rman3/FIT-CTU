/**
 * @file    rat.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "rat.h"

	void CRat::initializeRat(ObjectGeometry** geometry, ShaderProgram* shaderProgram) 
	{
		*geometry = new ObjectGeometry;

		//VAO
		glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
		glBindVertexArray((*geometry)->vertexArrayObject);

		//VBO
		glGenBuffers(1, &((*geometry)->vertexBufferObject));
		glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, ratNVertices * ratNAttribsPerVertex * sizeof(float), ratVertices, GL_STATIC_DRAW);

		//EBO
		glGenBuffers(1, &((*geometry)->elementBufferObject));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * ratNTriangles, ratIndices, GL_STATIC_DRAW);
		
		//VAO
		glEnableVertexAttribArray(shaderProgram->locations.position);
		glVertexAttribPointer(shaderProgram->locations.position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(shaderProgram->locations.normal);
		glVertexAttribPointer(shaderProgram->locations.normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(shaderProgram->locations.texCoord);
		glVertexAttribPointer(shaderProgram->locations.texCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		//init material and texture
		(*geometry)->ambient = glm::vec3(0.6f, 0.3f, 0.0f);
		(*geometry)->diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		(*geometry)->specular = glm::vec3(0.6f, 0.6f, 0.6f);
		(*geometry)->shininess = 0.4f;
		(*geometry)->texture = pgr::createTexture(RAT_TEXTURE_PATH);

		glBindVertexArray(0);
		(*geometry)->numTriangles = ratNTriangles;

	}

	void CRat::drawRat(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, ShaderProgram* shaderProgram) 
	{
		glUseProgram(shaderProgram->program);

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), init_pos);

		modelMatrix = glm::scale(modelMatrix, glm::vec3(size));

		if (!flipped)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
		else
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 15));

		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		setMaterialUniforms(rat_geo->ambient, rat_geo->diffuse, rat_geo->specular, rat_geo->shininess, rat_geo->texture);
		setLightsUniforms();
		setOtherUniforms();

		glBindVertexArray(rat_geo->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, rat_geo->numTriangles * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}
