/**
 * @file    camera.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once
#include "singlemesh.h"
#include "constants.h"

struct CCamera : public ObjectInstance
{
	float viewAngle;
	float verticalAngle;
	float horizontalAngle;
	glm::vec3 upVector;
	glm::vec3 center;
	glm::vec3 lastPos;
	bool inside;	// dynamic camera inside yaga house

	void initializeCamera(GameState& game_state);

	void updateVerticalAngle(float delta);

	void updateHorizontalAngle(float delta);

	// move to static view
	void to_point(int target[], int targetVAngle, int targetHAngle);

	// move depend on keys pressed
	void update(GameState& game_state);

	/**
	* Move to view 1, Z key pressed
	*/
	void view1update();

	/**
	* Move to view 2, X key pressed
	*/
	void view2update();

	/**
	* Check collision within cube borders and rat itself
	* @return true if collides, false if not
	*/
	bool collides_with_scene_border(glm::vec3& tmp);

	
};
