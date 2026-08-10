/**
 * @file    lights.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "pgr.h"
#include "lights.h"
#include "constants.h"

void CLights::initializeLights() {

	globalAmbientLight = glm::vec3(0.01f);

	// set up sun parameters
	sun.ambient = glm::vec3(SUN_AMBIENT);
	sun.diffuse = glm::vec3(SUN_DIFFUSE);
	sun.specular = glm::vec3(SUN_SPECULAR);
	sun.speed = SUN_SPEED;


	// set up reflector parameters
	spotLight.ambient = glm::vec3(SPOTLIGHT_AMBIENT);
	spotLight.diffuse = glm::vec3(SPOTLIGHT_DIFFUSE);
	spotLight.specular = glm::vec3(SPOTLIGHT_SPECULAR);
	spotLight.cosCutOff = SPOTLIGHT_COS_CUT_OFF;
	spotLight.exponent = SPOTLIGHT_EXPONENT;

}

void CLights::update(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, const GameState& game_state) 
{
	sun.direction = glm::vec3(cos(game_state.elapsed_time * sun.speed), 0.0, sin(game_state.elapsed_time * sun.speed));
	spotLight.position = cameraPosition;
	spotLight.direction = cameraDirection;
}
