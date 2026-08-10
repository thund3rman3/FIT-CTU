/**
 * @file    singlemesh.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "lights.h"
#include "objects_render.h"

class SingleMesh : public ObjectInstance
{
public:
	/**
	* Constructor
	* @param file_name - path to file location
	* @param shader
	*/
	SingleMesh(const std::string& file_name, ShaderProgram* shader = nullptr);

	~SingleMesh();

	void update(float elapsedTime, const glm::mat4* parentModelMatrix) override;

	/**
	* Draw object
	* @param viewMatrix
	* @param projectionMatrix
	*/
	void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:
	bool initialized;

	/**
	* Loads mesh with all its materials
	* @param file_name - path to file location
	* @param shader
	* @return true if loader, false otherwise
	*/
	bool loadSingleMesh(const std::string& file_name, ShaderProgram* shader);
};

