/**
 * @file    objects_render.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "pgr.h"
#include "constants.h"
#include <vector>
#include "lights.h"
#include "curve.h"
#include <iostream>
class ObjectInstance;

typedef std::vector<ObjectInstance*> ObjectList;

static ObjectList objects;

/**
 *  Shader program related stuff (id, locations, ...).
 */
struct ShaderProgram 
{
	ShaderProgram() : program(0), initialized(false) {
		locations.position = -1;
		locations.PVMmatrix = -1;
	}

	bool initialized;
	// identifier for the shader program
	GLuint program;          // = 0;

	/**
	  *  Indices of the vertex shader inputs (locations)
	  */
	struct {
		// vertex attributes locations
		GLint position;
		GLint normal;
		GLint texCoord;

		// uniforms locations
		GLint PVMmatrix;
		GLint Vmatrix;
		GLint Mmatrix;
		GLint normalMatrix;
		GLint time;
		GLint transparency;  // = -1;

		// materials
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint shininess;

		// texture
		GLint texSampler;
		GLint useTexture;
	} locations;

	// sun related uniforms
	GLint sunAmbientLocation;
	GLint sunDiffuseLocation;
	GLint sunSpecularLocation;
	GLint sunDirectionLocation;

	// spot light related uniforms
	GLint spotLightAmbientLocation;
	GLint spotLightDiffuseLocation;
	GLint spotLightSpecularLocation;
	GLint spotLightPositionLocation;
	GLint spotLightDirectionLocation; // = -1;
	GLint spotLightCosCutOffLocation;
	GLint spotLightExponentLocation;

	GLint globalAmbientLightLocation;

	GLint useFlashlightLocation;

	GLint fogModeLocation;
	GLint cameraPositionLocation;
} ;

/**
 *  Geometry of an object (vertices, triangles).
 */
struct ObjectGeometry //MeshGeometry
{
	GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
	GLuint        elementBufferObject;  ///< identifier for the element buffer object
	GLuint        vertexArrayObject;    ///< identifier for the vertex array object
	unsigned int  numTriangles;         ///< number of triangles in the mesh

	// material
	glm::vec3     ambient;
	glm::vec3     diffuse;
	glm::vec3     specular;
	float         shininess;
	GLuint        texture;
};

class ObjectInstance //object
{
public:
	std::vector<ObjectGeometry> geometry;
	glm::mat4		localModelMatrix;
	glm::mat4		globalModelMatrix;

	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     size;
	size_t	  id;
	bool destroyed;

	float startTime;
	float currentTime;
	float     rotationSpeed;
	glm::vec3 init_pos;

	ShaderProgram* shaderProgram;
	ObjectList children;

	/**
	 *  ObjectInstance constructor. Takes a pointer to the shader and must create object resources (VBO and VAO)
	 * @param shdrPrg pointer to the shader program for rendering objects data
	 */
	ObjectInstance(ShaderProgram* shdrPrg = nullptr) : shaderProgram(shdrPrg) {}

	~ObjectInstance() {}
  
	/**
	*  Recalculates the global matrix and updates all children.
	*   Derived classes should also call this method (using ObjectInstance::update()).
	* @param elapsedTime time value in seconds, such as 0.001*glutGet(GLUT_ELAPSED_TIME) (conversion milliseconds => seconds)
	* @param parentModelMatrix parent transformation in the scene-graph subtree
	*/
	virtual void update(const float elapsedTime, const glm::mat4* parentModelMatrix) 
	{
		if (id == 0)
		{
			currentTime = elapsedTime;
			float curveParamT = speed * (currentTime - startTime);
			position = init_pos + evaluateClosedCurve(curveData, curveSize, curveParamT);
			direction = glm::normalize(evaluateClosedCurve_1stDerivative(curveData, curveSize, curveParamT));
		}
	}

	/**
	 *  Draw instance geometry and calls the draw() on child nodes.
	 */
	virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		// process all children
		for (ObjectInstance* child : children) 
		{
			if (child != nullptr)
				child->draw(viewMatrix, projectionMatrix);
		}
	}
};

struct SkyboxFarPlaneShaderProgram {
	// identifier for the shader program
	GLuint program;                 // = 0;
	// vertex attributes locations
	GLint screenCoordLocation;      // = -1;
	// uniforms locations
	GLint inversePVmatrixLocation; // = -1;
	GLint skyboxSamplerLocation;    // = -1;
	GLint fogModeLocation;
	GLint time;
};

// Gets position of moving yaga house
glm::vec3 get_house_pos();

// Gets direction of moving yaga house
glm::vec3 get_house_dir();

// uniform getters
void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, GLuint texture);
void setLightsUniforms();
void setOtherUniforms();

//loads positions of all objects and sets some other variables from config.txt
bool load_from_file();
void initilizeSkybox(ObjectGeometry** geometry);
void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);


// Load and compile shader programs. Get attribute locations.
void loadShaderPrograms();
// Load models
void initializeModels();
// Draw all scene objects.
void drawScene(GameState& game_state);
// Delete all shader program objects.
void cleanupShaderPrograms();
// Delete geometry
void cleanupGeometry(ObjectGeometry* geometry);
void cleanupModels();
// Delete objects
void cleanUpObjects();
