/**
 * @file    camera.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "camera.h"
#include <iostream>
#include "constants.h"
#include "cb.h"
#include "rat.h"

extern CRat rat;

void CCamera::initializeCamera(GameState& game_state)
{
	lastPos = position = glm::vec3(10.0);
	viewAngle = 90.0f; // degrees
	currentTime = game_state.elapsed_time;
	direction = glm::vec3(cos(glm::radians(viewAngle)), sin(glm::radians(viewAngle)), 0.0f);
	verticalAngle = 0.0f;
	horizontalAngle = 0.0f;
	center = position + direction;
	inside = false;
}

void CCamera::updateVerticalAngle(float delta) {

	// keep angle alvays in < - CAMERA_MAX_ELEVATION_ANGLE , + CAMERA_MAX_ELEVATION_ANGLE >
	if (fabs(verticalAngle - delta) < CAMERA_ELEVATION_MAX)
		verticalAngle -= delta;

}

void CCamera::updateHorizontalAngle(float delta) {

	horizontalAngle -= delta;
	// keep angle alvays in <0, 360>
	if (horizontalAngle < 0)
		horizontalAngle += 360;
	else if (horizontalAngle > 360)
		horizontalAngle -= 360;

}

bool CCamera::collides_with_scene_border(glm::vec3& tmp)
{
	if ((int)tmp.x >= SCENE_X_MIN && (int)tmp.x <= SCENE_X_MAX &&
		(int)tmp.y >= SCENE_Y_MIN && (int)tmp.y <= SCENE_Y_MAX &&
		(int)tmp.z >= SCENE_Z_MIN && (int)tmp.z <= SCENE_Z_MAX &&
		glm::distance(tmp, rat.init_pos) > COLLISION_RADIUS)
		return true;
	return false;
}

void CCamera::update(GameState& game_state) {

	if (game_state.keys[KEY_X] == true || game_state.keys[KEY_Z] == true || inside)
		game_state.camera_free = false;
	else
		game_state.camera_free = true;

	float alpha = glm::radians(verticalAngle);
	float beta = glm::radians(horizontalAngle);

	direction = glm::vec3(
		cos(alpha) * cos(beta),
		cos(alpha) * sin(beta),
		sin(alpha)
	);
	upVector = glm::vec3(
		-sin(alpha) * cos(beta),
		-sin(alpha) * sin(beta),
		cos(alpha)
	);

	// cross product of upVector and viewDirection will give us orthogonal vector to this plane (vector to the side)
	glm::vec3 cameraSideVector = glm::cross(direction, upVector);

	if (game_state.keys[KEY_Z] == true)
		view1update();
	else if (game_state.keys[KEY_X] == true)
		view2update();
	else
	{
		position = lastPos;
		
		if (game_state.keys[KEY_UP_ARROW] == true)
		{
			glm::vec3 tmp = position + direction * CAMERA_MOVEMENT_COEFF;
			if (collides_with_scene_border(tmp))
			{
				position += direction * CAMERA_MOVEMENT_COEFF;
			}
		}
		if (game_state.keys[KEY_DOWN_ARROW] == true)
		{
			glm::vec3 tmp = position - direction * CAMERA_MOVEMENT_COEFF;
			if (collides_with_scene_border(tmp))
			{
				position -= direction * CAMERA_MOVEMENT_COEFF;
			}
		}

		if (game_state.keys[KEY_RIGHT_ARROW] == true)
		{
			glm::vec3 tmp = position + cameraSideVector * CAMERA_MOVEMENT_COEFF;
			if (collides_with_scene_border(tmp))
			{
				position += cameraSideVector * CAMERA_MOVEMENT_COEFF;
			}
		}

		if (game_state.keys[KEY_LEFT_ARROW] == true)
		{
			glm::vec3 tmp = position - cameraSideVector * CAMERA_MOVEMENT_COEFF;
			if (collides_with_scene_border(tmp))
			{
				position -= cameraSideVector * CAMERA_MOVEMENT_COEFF;
			}
		}
		lastPos = position;
	}
	
	center = position + direction;

}

void posUpdate(float& a, int target, float tr)
{
	if ((int)a > target)
	{
		a -= tr;
		if ((int)a <= target)
		{
			a = target;
		}
	}
	else if ((int)a < target)
	{
		a += tr;
		if ((int)a >= target) 
		{ a = target; }
	}
	std::cerr << a << " "<< target << std::endl;
}

void CCamera::to_point(int target[], int targetVAngle, int targetHAngle)
{
	float tr = 10.1f;
	if ((int)position[0] != target[0])
		posUpdate(position.x, target[0], tr);
	if((int)position[1] != target[1])
		posUpdate(position.y, target[1], tr);
	if((int)position[2] != target[2])
		posUpdate(position.z, target[2], tr);
	if((int)verticalAngle != targetVAngle)
		posUpdate(verticalAngle, targetVAngle, tr);
	if(horizontalAngle != targetHAngle)
		posUpdate(horizontalAngle, targetHAngle, tr);

		std::cerr << "pos :" << position[0] << " " << position[1] << " " << position[2] << ", " << verticalAngle << std::endl;
}

void CCamera::view1update()
{
	int target[3] = { -60,87,31 };
	int targetVAngle = 0;
	int targetHAngle = 315;
	to_point(target, targetVAngle, targetHAngle);

}

void CCamera::view2update()
{
	int target[3] = { 220,230,60 };
	int targetVAngle = -10;
	int targetHAngle = 220;
	to_point(target, targetVAngle, targetHAngle);
}
