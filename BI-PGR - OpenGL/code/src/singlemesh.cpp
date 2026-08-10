/**
 * @file    singlemesh.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include <iostream>
#include "singlemesh.h"
#include "constants.h"
#include "lights.h"
#include "objects_render.h"
#include "camera.h"
#include "curve.h"

extern CCamera camera;
extern GameState game_state;

SingleMesh::SingleMesh(const std::string& file_name, ShaderProgram* shader) : ObjectInstance(shader), initialized(false)
{

	if (!loadSingleMesh(file_name, shader)) {
		for (size_t i = 0; i < geometry.size(); ++i)
		{
			if (geometry[i].vertexArrayObject == false) {
				std::cerr << "SingleMesh::SingleMesh(): geometry not initialized!" << std::endl;
			}
			else {
				std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
			}
		}
	}
	else {
		if ((shaderProgram != nullptr) && shaderProgram->initialized && (shaderProgram->locations.PVMmatrix != -1)) {
			initialized = true;
		}
		else {
			std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
		}
	}
}

SingleMesh::~SingleMesh() {
	for(size_t i = 0; i< geometry.size();++i)
	{
		glDeleteVertexArrays(1, &(geometry[i].vertexArrayObject));
			glDeleteBuffers(1, &(geometry[i].elementBufferObject));
			glDeleteBuffers(1, &(geometry[i].vertexBufferObject));
			std::cerr << "mesh deleted" << std::endl;
	}
	initialized = false;
}

void SingleMesh::update(float elapsedTime, const glm::mat4* parentModelMatrix) {
	ObjectInstance::update(elapsedTime, parentModelMatrix);
}

void SingleMesh::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
		glStencilFunc(GL_ALWAYS, id+1, 0);
		if (initialized && (shaderProgram != nullptr))
		{
			glUseProgram(shaderProgram->program);
			glm::mat4 modelMatrix;

			if(id==0)
				modelMatrix = alignObject(position, direction, glm::vec3(0.0f, 0.0f, 1.0f));
			else
			{
				modelMatrix = glm::translate(glm::mat4(1.0f), this->init_pos);
				modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
				if (id == 3)
					modelMatrix = glm::rotate(modelMatrix, glm::radians(240.0f), glm::vec3(0, 1, 0));
			}
			modelMatrix = glm::scale(modelMatrix, glm::vec3(this->size));


			// send matrices to the vertex & fragment shader
			setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
			setLightsUniforms();
			setOtherUniforms();
			CHECK_GL_ERROR();

			for (int i = 0; i < this->geometry.size(); i++)
			{
				setMaterialUniforms(
					this->geometry.at(i).ambient,
					this->geometry.at(i).diffuse,
					this->geometry.at(i).specular,
					this->geometry.at(i).shininess,
					this->geometry.at(i).texture
				);
				CHECK_GL_ERROR();
				//draw all geometries
				glBindVertexArray(this->geometry.at(i).vertexArrayObject);
				glDrawElements(GL_TRIANGLES, this->geometry.at(i).numTriangles * 3, GL_UNSIGNED_INT, 0);
			}
			CHECK_GL_ERROR();
			glBindVertexArray(0);
			glUseProgram(0);
		}
		else 
		{
			std::cerr << "SingleMesh::draw(): Can't draw, mesh not initialized properly!" << std::endl;
		}
}

bool SingleMesh::loadSingleMesh(const std::string& fileName, ShaderProgram* shader) {
	Assimp::Importer importer;

	// unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // triangulate polygons (if any)
		| aiProcess_PreTransformVertices    // transforms scene hierarchy into one root with geometry-leafs only, for more see Doc
		| aiProcess_GenSmoothNormals        // calculate normals per vertex
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "SingleMesh::loadSingleMesh(): assimp error - " << importer.GetErrorString() << std::endl;
		return false;
	}

	bool validInit = false;

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	for (size_t i = 0; i < scn->mNumMeshes; i++) {
		const aiMesh* mesh = scn->mMeshes[i];
		geometry.push_back(ObjectGeometry());

		// vertex buffer object, store all vertex positions
		glGenBuffers(1, &(geometry[i].vertexBufferObject));
		glBindBuffer(GL_ARRAY_BUFFER, geometry[i].vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW);     // allocate memory for vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices); // store all vertices
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals); // store normals

		// just texture 0 for now
		float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
		float* currentTextureCoord = textureCoords;
		aiVector3D vect;

		if (mesh->HasTextureCoords(0)) {
			// 2D textures with 2 coordinates
			for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
				vect = (mesh->mTextureCoords[0])[idx];
				*currentTextureCoord++ = vect.x;
				*currentTextureCoord++ = vect.y;
			}
		}

		// store texture coordinates
		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

		// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		}

		// copy our temporary index array to OpenGL and free the array
		glGenBuffers(1, &(geometry[i].elementBufferObject));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry[i].elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

		delete[] indices;

		// copy the material info to MeshGeometry structure
		const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
		aiColor4D color;
		aiString name;
		aiReturn retValue = AI_SUCCESS;


		// get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
		mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing, must be aiString type!

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry[i].diffuse = glm::vec3(color.r, color.g, color.b);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry[i].ambient = glm::vec3(color.r, color.g, color.b);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry[i].specular = glm::vec3(color.r, color.g, color.b);

		ai_real shininess, strength;
		unsigned int max;

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;
		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;
		geometry[i].shininess = shininess * strength;

		geometry[i].texture = 0;

		// load texture image
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			// get texture name 
			aiString path; // filename

			aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");
			// insert correct texture file path 
			if (found != std::string::npos) { // not found
				//subMesh_p->textureName.insert(0, "/");
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading texture file: " << textureName << std::endl;
			geometry[i].texture = pgr::createTexture(textureName);
		}
		CHECK_GL_ERROR();

		glGenVertexArrays(1, &(geometry[i].vertexArrayObject));
		glBindVertexArray(geometry[i].vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry[i].elementBufferObject); // bind our element array buffer (indices) to vao
		glBindBuffer(GL_ARRAY_BUFFER, geometry[i].vertexBufferObject);

		if ((shaderProgram != nullptr) && shaderProgram->initialized && (shaderProgram->locations.position != -1)) {

			glEnableVertexAttribArray(shader->locations.position);
			glVertexAttribPointer(shader->locations.position, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(shader->locations.normal);
			glVertexAttribPointer(shader->locations.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

			CHECK_GL_ERROR();

			validInit = true;
		}

		glEnableVertexAttribArray(shader->locations.texCoord);
		glVertexAttribPointer(shader->locations.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));

		glBindVertexArray(0);

		geometry[i].numTriangles = mesh->mNumFaces;
	}

	return validInit;
}



