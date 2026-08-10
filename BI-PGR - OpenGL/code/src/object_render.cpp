/**
 * @file    objects_render.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "pgr.h"
#include "objects_render.h"
#include "constants.h"
#include <iostream>
#include "singlemesh.h"
#include "camera.h"
#include "lights.h"
#include "rat.h"
#include <sstream>
#include <fstream>
#include "smoke.h"
#include "fire.h"

ShaderProgram shaderProgram;
CCamera camera;
GameState game_state;
CLights lights;
CRat rat;
ObjectGeometry* skybox_geo = nullptr;
CSmoke smoke;
CFire fire;

SkyboxFarPlaneShaderProgram skyboxFarPlaneShaderProgram;


glm::vec3 get_house_pos()
{
	return objects[0]->position + objects[0]->direction*8.0f + glm::vec3(0.0,0.0,3.0);
}

glm::vec3 get_house_dir()
{
	std::cerr << "get budka dir :" << objects[0]->direction[0] << " " << objects[0]->direction[1] << " " << objects[0]->direction[2] << std::endl;
	return objects[0]->direction;
}

void initilizeSkybox(ObjectGeometry** geometry) {

	*geometry = new ObjectGeometry;

	// 2D coordinates of 2 triangles covering the whole screen (NDC), draw using triangle strip
	static const float screenCoords[] = {
	  -1.0f, -1.0f,
	   1.0f, -1.0f,
	  -1.0f,  1.0f,
	   1.0f,  1.0f
	};

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// buffer for far plane rendering
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	//glUseProgram(farplaneShaderProgram);

	glEnableVertexAttribArray(skyboxFarPlaneShaderProgram.screenCoordLocation);
	glVertexAttribPointer(skyboxFarPlaneShaderProgram.screenCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();

	(*geometry)->numTriangles = 2;

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &((*geometry)->texture));
	glBindTexture(GL_TEXTURE_CUBE_MAP, (*geometry)->texture);

	const char* suffixes[] = { "right", "left", "bot", "top", "front", "back" };
	//const char* suffixes[] = { "bot", "top", "right", "left" ,"front","back"  };
	GLuint targets[] = {
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_CUBE_TEXTURE_FILE_PREFIX) + "_" + suffixes[i] + ".png";
		std::cout << "Loading cube map texture: " << texName << std::endl;
		if (!pgr::loadTexImage2D(texName, targets[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();

}

void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(skyboxFarPlaneShaderProgram.program);

	// compose transformations
	glm::mat4 matrix = projectionMatrix * viewMatrix;

	// create view rotation matrix by using view matrix with cleared translation
	glm::mat4 viewRotation = viewMatrix;
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// vertex shader will translate screen space coordinates (NDC) using inverse PV matrix
	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);

	glUniformMatrix4fv(skyboxFarPlaneShaderProgram.inversePVmatrixLocation, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skyboxFarPlaneShaderProgram.skyboxSamplerLocation, 0);
	glUniform1i(skyboxFarPlaneShaderProgram.fogModeLocation, game_state.fog);
	glUniform1f(skyboxFarPlaneShaderProgram.time, game_state.elapsed_time);


	// draw "skybox" rendering 2 triangles covering the far plane
	glBindVertexArray(skybox_geo->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_geo->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, skybox_geo->numTriangles + 2);

	glBindVertexArray(0);
	glUseProgram(0);
}

bool load_from_file()
{
	std::ifstream ifs("config.txt", std::ifstream::in);
	if (!ifs.is_open())
		return false;
	std::string line;
	std::getline(ifs, line);

	if (line.empty())
		return false;

	//first loaded object is rat
	std::stringstream sstr(line);
	sstr >> rat.init_pos[0] >> rat.init_pos[1] >> rat.init_pos[2];
	rat.size = 0.1f;
	std::cerr << rat.init_pos[0] << " " << rat.init_pos[1] << " " << rat.init_pos[2] << std::endl;

	//load other objects
	for (size_t i = 0; ifs.good() && i < objects.size(); i++)
	{
		std::getline(ifs, line);

		std::stringstream sstr(line);
		sstr >> objects[i]->init_pos[0] >> objects[i]->init_pos[1] >> objects[i]->init_pos[2];

		objects[i]->position = objects[i]->init_pos;

		objects[i]->id = i;

		objects[i]->currentTime = objects[i]->startTime = game_state.elapsed_time;

		std::cerr << objects[i]->init_pos[0] << " " << objects[i]->init_pos[1] <<
			" " << objects[i]->init_pos[2] <<
			" " << objects[i]->id << std::endl;
	}
	if(!line.empty())
		std::getline(ifs, line);
	std::cerr << line << std::endl;
	std::getline(ifs, line);

	while (!line.empty()) // load fire size or smoke size
	{
		std::cerr << line << std::endl;
		std::stringstream sstr(line);
		std::string str;
		sstr >> str;
		if (str == "fire_size")
			sstr >> fire.fire_size;
		else if (str == "smoke_size")
			sstr >> smoke.smoke_size;
		std::getline(ifs, line);
	}
	return true;
}

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

    glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(shaderProgram.locations.PVMmatrix, 1, GL_FALSE, glm::value_ptr(PVM));
    glUniformMatrix4fv(shaderProgram.locations.Vmatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shaderProgram.locations.Mmatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // just take 3x3 rotation part of the modelMatrix
    // we presume the last row contains 0,0,0,1
    const glm::mat4 modelRotationMatrix = glm::mat4(
        modelMatrix[0],
        modelMatrix[1],
        modelMatrix[2],
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

    //or an alternative single-line method: 
    //glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat4(glm::mat3(modelRotationMatrix))));

    glUniformMatrix4fv(shaderProgram.locations.normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));  // correct matrix for non-rigid transform
	CHECK_GL_ERROR();
}

void setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, GLuint texture) {

    glUniform3fv(shaderProgram.locations.diffuse, 1, glm::value_ptr(diffuse));  // 2nd parameter must be 1 - it declares number of vectors in the vector array
    glUniform3fv(shaderProgram.locations.ambient, 1, glm::value_ptr(ambient));
    glUniform3fv(shaderProgram.locations.specular, 1, glm::value_ptr(specular));
    glUniform1f(shaderProgram.locations.shininess, shininess);

    if (texture != 0) {
        glUniform1i(shaderProgram.locations.useTexture, 1);  // do texture sampling
        glUniform1i(shaderProgram.locations.texSampler, 0);  // texturing unit 0 -> samplerID   [for the GPU linker]
        glActiveTexture(GL_TEXTURE0 + 0);                  // texturing unit 0 -> to be bound [for OpenGL BindTexture]
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    else {
        glUniform1i(shaderProgram.locations.useTexture, 0);  // do not sample the texture
    }
	CHECK_GL_ERROR();
}

void setLightsUniforms() 
{
	// sun
	glUniform3fv(shaderProgram.sunAmbientLocation, 1, glm::value_ptr(lights.sun.ambient));
	glUniform3fv(shaderProgram.sunDiffuseLocation, 1, glm::value_ptr(lights.sun.diffuse));
	glUniform3fv(shaderProgram.sunSpecularLocation, 1, glm::value_ptr(lights.sun.specular));
	glUniform3fv(shaderProgram.sunDirectionLocation, 1, glm::value_ptr(lights.sun.direction));
	CHECK_GL_ERROR();

	// spot light
	glUniform3fv(shaderProgram.spotLightAmbientLocation, 1, glm::value_ptr(lights.spotLight.ambient));
	glUniform3fv(shaderProgram.spotLightDiffuseLocation, 1, glm::value_ptr(lights.spotLight.diffuse));
	glUniform3fv(shaderProgram.spotLightSpecularLocation, 1, glm::value_ptr(lights.spotLight.specular));
	glUniform3fv(shaderProgram.spotLightPositionLocation, 1, glm::value_ptr(lights.spotLight.position));
	glUniform3fv(shaderProgram.spotLightDirectionLocation, 1, glm::value_ptr(lights.spotLight.direction));
	glUniform1f(shaderProgram.spotLightCosCutOffLocation, lights.spotLight.cosCutOff);
	glUniform1f(shaderProgram.spotLightExponentLocation, lights.spotLight.exponent);

	glUniform3fv(shaderProgram.globalAmbientLightLocation, 1, glm::value_ptr(lights.globalAmbientLight));

	CHECK_GL_ERROR();
}

void setOtherUniforms() 
{
	glUniform1i(shaderProgram.useFlashlightLocation, game_state.useFlashlight);
	glUniform1f(shaderProgram.locations.time, game_state.elapsed_time);
	glUniform1i(shaderProgram.fogModeLocation, game_state.fog);
	glUniform3fv(shaderProgram.cameraPositionLocation, 1, glm::value_ptr(camera.position));
	CHECK_GL_ERROR();
}

void loadShaderPrograms()
{
	std::vector<GLuint> shaders;

	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader.frag"));

	shaderProgram.program = pgr::createProgram(shaders);

	shaderProgram.locations.position = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.locations.normal = glGetAttribLocation(shaderProgram.program, "normal");
	shaderProgram.locations.texCoord = glGetAttribLocation(shaderProgram.program, "texCoord");

	shaderProgram.locations.PVMmatrix = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.locations.Vmatrix = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	shaderProgram.locations.Mmatrix = glGetUniformLocation(shaderProgram.program, "Mmatrix");
	shaderProgram.locations.normalMatrix = glGetUniformLocation(shaderProgram.program, "normalMatrix");
	shaderProgram.locations.time = glGetUniformLocation(shaderProgram.program, "time");

	// material
	shaderProgram.locations.ambient = glGetUniformLocation(shaderProgram.program, "material.ambient");
	shaderProgram.locations.diffuse = glGetUniformLocation(shaderProgram.program, "material.diffuse");
	shaderProgram.locations.specular = glGetUniformLocation(shaderProgram.program, "material.specular");
	shaderProgram.locations.shininess = glGetUniformLocation(shaderProgram.program, "material.shininess");

	// texture
	shaderProgram.locations.texSampler = glGetUniformLocation(shaderProgram.program, "texSampler");
	shaderProgram.locations.useTexture = glGetUniformLocation(shaderProgram.program, "material.useTexture");

	// sun
	shaderProgram.sunAmbientLocation = glGetUniformLocation(shaderProgram.program, "sun.ambient");
	shaderProgram.sunDiffuseLocation = glGetUniformLocation(shaderProgram.program, "sun.diffuse");
	shaderProgram.sunSpecularLocation = glGetUniformLocation(shaderProgram.program, "sun.specular");
	shaderProgram.sunDirectionLocation = glGetUniformLocation(shaderProgram.program, "sun.direction");
	CHECK_GL_ERROR();

	// spot light 
	shaderProgram.spotLightAmbientLocation = glGetUniformLocation(shaderProgram.program, "spotLight.ambient");
	shaderProgram.spotLightDiffuseLocation = glGetUniformLocation(shaderProgram.program, "spotLight.diffuse");
	shaderProgram.spotLightSpecularLocation = glGetUniformLocation(shaderProgram.program, "spotLight.specular");
	shaderProgram.spotLightPositionLocation = glGetUniformLocation(shaderProgram.program, "spotLight.position");
	shaderProgram.spotLightDirectionLocation = glGetUniformLocation(shaderProgram.program, "spotLight.direction");
	shaderProgram.spotLightCosCutOffLocation = glGetUniformLocation(shaderProgram.program, "spotLight.cosCutOff");
	shaderProgram.spotLightExponentLocation = glGetUniformLocation(shaderProgram.program, "spotLight.exponent");

	shaderProgram.globalAmbientLightLocation = glGetUniformLocation(shaderProgram.program, "globalAmbientLight");
	shaderProgram.useFlashlightLocation = glGetUniformLocation(shaderProgram.program, "useFlashlight");

	shaderProgram.cameraPositionLocation = glGetUniformLocation(shaderProgram.program, "cameraPosition");
	shaderProgram.fogModeLocation = glGetUniformLocation(shaderProgram.program, "fog");
	CHECK_GL_ERROR();

	assert(shaderProgram.locations.PVMmatrix != -1);
	assert(shaderProgram.locations.position != -1);

	shaderProgram.initialized = true;

	shaders.clear();

	// SKYBOX SHADER
	// push vertex shader and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skybox.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skybox.frag"));

	// create the program with two shaders
	skyboxFarPlaneShaderProgram.program = pgr::createProgram(shaders);

	// handles to vertex attributes locations
	skyboxFarPlaneShaderProgram.screenCoordLocation = glGetAttribLocation(skyboxFarPlaneShaderProgram.program, "screenCoord");
	// get uniforms locations
	skyboxFarPlaneShaderProgram.skyboxSamplerLocation = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "skyboxSampler");
	skyboxFarPlaneShaderProgram.inversePVmatrixLocation = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "inversePVmatrix");
	skyboxFarPlaneShaderProgram.fogModeLocation = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "fog");
	skyboxFarPlaneShaderProgram.time = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "time");
	CHECK_GL_ERROR();
	shaders.clear();

	// FIRE SHADER
	// push vertex shader and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fire.frag"));
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "fire.vert"));

	// create the program with two shaders
	fire.fireShaderProgram.program = pgr::createProgram(shaders);

	// get position and texture coordinates attributes locations
	fire.fireShaderProgram.posLocation = glGetAttribLocation(fire.fireShaderProgram.program, "position");
	fire.fireShaderProgram.texCoordLocation = glGetAttribLocation(fire.fireShaderProgram.program, "texCoord");
	// get uniforms locations
	fire.fireShaderProgram.PVMmatrixLocation = glGetUniformLocation(fire.fireShaderProgram.program, "PVMmatrix");
	fire.fireShaderProgram.VmatrixLocation = glGetUniformLocation(fire.fireShaderProgram.program, "Vmatrix");
	fire.fireShaderProgram.timeLocation = glGetUniformLocation(fire.fireShaderProgram.program, "time");
	fire.fireShaderProgram.texSamplerLocation = glGetUniformLocation(fire.fireShaderProgram.program, "texSampler");
	fire.fireShaderProgram.frameDurationLocation = glGetUniformLocation(fire.fireShaderProgram.program, "frameDuration");
	CHECK_GL_ERROR();
	shaders.clear();

	// SMOKE SHADER
	// push vertex shader and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "smoke.frag"));
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "smoke.vert"));

	// create the program with two shaders
	smoke.smokeShaderProgram.program = pgr::createProgram(shaders);
	// position and texture coordinates attributes locations
	smoke.smokeShaderProgram.positionLocation = glGetAttribLocation(smoke.smokeShaderProgram.program, "position");
	smoke.smokeShaderProgram.texCoordLocation = glGetAttribLocation(smoke.smokeShaderProgram.program, "texCoord");

	// uniforms locations
	smoke.smokeShaderProgram.PVMmatrixLocation = glGetUniformLocation(smoke.smokeShaderProgram.program, "PVMmatrix");
	smoke.smokeShaderProgram.timeLocation = glGetUniformLocation(smoke.smokeShaderProgram.program, "time");
	smoke.smokeShaderProgram.texSamplerLocation = glGetUniformLocation(smoke.smokeShaderProgram.program, "texSampler");

	shaders.clear();
}

void initializeModels()
{
	camera.initializeCamera(game_state);
	initilizeSkybox(&skybox_geo);
	rat.initializeRat(&rat.rat_geo, &shaderProgram);
	rat.size = 0.1f;
	lights.initializeLights();


	objects.push_back(new SingleMesh(YAGA_PATH, &shaderProgram));
	objects[0]->size = 25.0f;
	objects[0]->speed = (float)(rand() / (double)RAND_MAX) * 100;

	objects.push_back(new SingleMesh(PLANE_PATH, &shaderProgram));
	objects[1]->size = 400.0f;

	objects.push_back(new SingleMesh(CAMPFIRE_PATH, &shaderProgram));
	objects[2]->size = 5.0f;

	objects.push_back(new SingleMesh(YAGA_PATH, &shaderProgram));
	objects[3]->size = 25.0f;

	objects.push_back(new SingleMesh(STONE_PATH, &shaderProgram));
	objects[4]->size = 5.0f;

	objects.push_back(new SingleMesh(CHEST_PATH, &shaderProgram));
	objects[5]->size = 7.0f;

	objects.push_back(new SingleMesh(JACK_PATH, &shaderProgram));
	objects[6]->size = 5.0f;

	objects.push_back(new SingleMesh(GRASS_PATH, &shaderProgram));
	objects[7]->size = 3.0f;

	objects.push_back(new SingleMesh(SKELETON_PATH, &shaderProgram));
	objects[8]->size = 8.0f;

	objects.push_back(new SingleMesh(WOLF_PATH, &shaderProgram));
	objects[9]->size = 12.0f;

	objects.push_back(new SingleMesh(AST_PATH, &shaderProgram));
	objects[10]->size = 25.0f;

	if (load_from_file() == false)
		std::cerr << "config in bad format" << std::endl;

	fire.initializeFire();
	smoke.initializeSmoke();
	game_state.firing = true;
	fire.fire_position = objects[2]->init_pos + glm::vec3(0, 0, 3);
	smoke.smoke_position = objects[2]->init_pos + glm::vec3(0, 0, 11);

	std::cerr << game_state.firing << "firing" << std::endl;
	std::cerr << fire.fire_size << "fire size" << std::endl;
	std::cerr << smoke.smoke_size << "smoke size" << std::endl;
	std::cerr << fire.fire_position[0] << " "<< fire.fire_position[1]<<
		" " << fire.fire_position[2] << std::endl;
	CHECK_GL_ERROR();
}

void drawScene( GameState& game_state)
{
	if (game_state.keys[KEY_R] == true)
		if (load_from_file() == false)
			std::cerr << "config in bad format" << std::endl;

	glm::mat4 viewMatrix = glm::lookAt(
		camera.position,
		camera.center,
		camera.upVector);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.viewAngle), game_state.windowWidth / (float)game_state.windowHeight, FRUSTUM_NEAR_PLANE, FRUSTUM_FAR_PLANE);
	
	drawSkybox(viewMatrix, projectionMatrix);

	glStencilFunc(GL_ALWAYS, 9, 0);
	rat.drawRat(viewMatrix, projectionMatrix, &shaderProgram);

	for (ObjectInstance* object : objects) 
	{   
		if (object != nullptr)
			object->draw(viewMatrix, projectionMatrix);
	}

	if (game_state.firing) 
	{
		glStencilFunc(GL_ALWAYS, 10, 0);
		fire.drawFire(viewMatrix, game_state.elapsed_time + 1, projectionMatrix);
		glStencilFunc(GL_ALWAYS, 11, 0);
		smoke.drawSmoke(viewMatrix, projectionMatrix, game_state);
	}

	// update the application state
	const glm::mat4 sceneRootMatrix = glm::mat4(1.0f);
	for (ObjectInstance* object : objects)
	{
		if (object != nullptr)
			object->update(game_state.elapsed_time, &sceneRootMatrix);
	}
	
}

void cleanupShaderPrograms(void)
{
	pgr::deleteProgramAndShaders(shaderProgram.program);
	pgr::deleteProgramAndShaders(skyboxFarPlaneShaderProgram.program);
	pgr::deleteProgramAndShaders(fire.fireShaderProgram.program);
	pgr::deleteProgramAndShaders(smoke.smokeShaderProgram.program);
}

void cleanupGeometry(ObjectGeometry* geometry)
{
	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));
	std::cerr << "geo deleted" << std::endl;
	if (geometry->texture != 0)
		glDeleteTextures(1, &(geometry->texture));
}

void cleanupModels() 
{
	cleanupGeometry(skybox_geo);
	cleanupGeometry(rat.rat_geo);
	// other geo should get destroyed by singlemesh destructor
}

void cleanUpObjects()
{
	for (ObjectInstance* object : objects)
	{
		std::cerr << "object deleted" << std::endl;
		delete object;
		object = nullptr;
	}
}
