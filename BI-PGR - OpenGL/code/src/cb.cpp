/**
 * @file    cb.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "pgr.h"
#include "cb.h"
#include "objects_render.h"
#include "constants.h"
#include <iostream>
#include "camera.h"
#include "rat.h"

extern GameState game_state;
extern CCamera camera;
extern CLights lights;
extern ObjectList objects;
extern CRat rat;

void gameMenu(int choice)
{
	switch (choice)
	{
	case 0:
		game_state.fog = !game_state.fog;
		break;
	case 1:
		game_state.useFlashlight = !game_state.useFlashlight;
		break;
	case 2:
		game_state.keys[KEY_Z] = !game_state.keys[KEY_Z];
		game_state.keys[KEY_X] = false;
		break;
	case 3:
		game_state.keys[KEY_X] = !game_state.keys[KEY_X];
		game_state.keys[KEY_Z] = false;
		break;
	case 4:
		camera.inside = true;
		game_state.camera_free = false;
		camera.position = get_house_pos();
		camera.direction = get_house_dir();
		camera.center = camera.position + camera.direction;
		std::cerr << "riding" << std::endl;
		break;
	case 5:
		game_state.firing = !game_state.firing;
		break;
	case 6:
		glutLeaveMainLoop();
		break;
	}
}

void createMenu(void) 
{
	glutCreateMenu(gameMenu);
	glutAddMenuEntry("Fog", 0);
	glutAddMenuEntry("Spotlight reflector", 1);
	glutAddMenuEntry("First view", 2);
	glutAddMenuEntry("Second view", 3);
	glutAddMenuEntry("Yaga house view", 4);
	glutAddMenuEntry("Fire", 5);
	glutAddMenuEntry("Exit game", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void displayCb() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// draw the window contents (scene objects)
	drawScene(game_state);

	glDisable(GL_STENCIL_TEST);

	glutSwapBuffers();
}

void reshapeCb(int newWidth, int newHeight) 
{
	game_state.windowWidth = newWidth;
	game_state.windowHeight = newHeight;
	glViewport(0, 0, game_state.windowWidth, game_state.windowHeight);
};

void keyboardCb(unsigned char keyPressed, int mouseX, int mouseY) 
{
	switch (keyPressed) 
	{
		case 27:
			glutLeaveMainLoop();
			exit(EXIT_SUCCESS);
			break;
		case 'r':
			game_state.keys[KEY_R] = true;
			game_state.keys[KEY_Z] = false;
			game_state.keys[KEY_X] = false;
			break;
		case 'x':
			game_state.keys[KEY_X] = !game_state.keys[KEY_X];
			game_state.keys[KEY_Z] = false;
			break;
		case 'z':
			game_state.keys[KEY_Z] = !game_state.keys[KEY_Z];
			game_state.keys[KEY_X] = false;
			break;
		case 'c':
		case 13:
			game_state.useFlashlight = !game_state.useFlashlight;
			std::cerr << "light switched" << std::endl;
			break;
		case 'f':
			game_state.fog = !game_state.fog;
			std::cerr << "fog switched" << std::endl;
			break;
		case 'd':
			game_state.keys[KEY_RIGHT_ARROW] = true;
			break;
		case 'a':
			game_state.keys[KEY_LEFT_ARROW] = true;
			break;
		case 'w':
			game_state.keys[KEY_UP_ARROW] = true;
			break;
		case 's':
			game_state.keys[KEY_DOWN_ARROW] = true;
			break;
		default:
			printf("Unrecognized key pressed\n");
	}

}


void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY) {
	switch (keyReleased)
	{
		case 'r':
			game_state.keys[KEY_R] = false;
			break;
		case 'd':
			game_state.keys[KEY_RIGHT_ARROW] = false;
			break;
		case 'a':
			game_state.keys[KEY_LEFT_ARROW] = false;
			break;
		case 'w':
			game_state.keys[KEY_UP_ARROW] = false;
			break;
		case 's':
			game_state.keys[KEY_DOWN_ARROW] = false;
			break;
	}
}


void specialKeyboardCb(int specKeyPressed, int mouseX, int mouseY) 
{

	switch (specKeyPressed) 
	{
		case GLUT_KEY_RIGHT:
			game_state.keys[KEY_RIGHT_ARROW] = true;
			break;
		case GLUT_KEY_LEFT:
			game_state.keys[KEY_LEFT_ARROW] = true;
			break;
		case GLUT_KEY_UP:
			game_state.keys[KEY_UP_ARROW] = true;
			break;
		case GLUT_KEY_DOWN:
			game_state.keys[KEY_DOWN_ARROW] = true;
			break;
		default:
			printf("Unrecognized special key pressed\n");
	}
}

void specialKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY) 
{
	switch (specKeyReleased)
	{
	case GLUT_KEY_RIGHT:
		game_state.keys[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		game_state.keys[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		game_state.keys[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		game_state.keys[KEY_DOWN_ARROW] = false;
		break;
	}

}

void mouseCb(int buttonPressed, int buttonState, int mouseX, int mouseY) 
{
	// interaction with left button down
	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)) {
		int objectID = 0;
		glReadPixels(mouseX, game_state.windowHeight - mouseY - 1,
			1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);

		std::cerr << objectID << " objectID" << std::endl;
		if (objectID == 9) 
		{
			rat.flipped = !rat.flipped;
			std::cerr << "rat flipped" << std::endl;
		}
		else if (objectID == 3) 
		{
			game_state.firing = !game_state.firing;
			std::cerr << "fire flipped" << std::endl;
		}
		else if (objectID == 1) 
		{ 
			camera.inside = true;
			game_state.camera_free = false;
			camera.position = get_house_pos();
			camera.direction = get_house_dir();
			camera.center = camera.position + camera.direction;
			std::cerr << "riding" << std::endl;
		}
	}
	else if ((buttonPressed == GLUT_MIDDLE_BUTTON) && (buttonState == GLUT_DOWN))
	{
		camera.inside = false;
		game_state.camera_free = true;
	}
}

void passiveMouseMotionCb(int mouseX, int mouseY) {

	if (mouseX != game_state.windowWidth / 2) {

		float deltaHorizontal = (mouseX - game_state.windowWidth / 2) * 0.3f;
		if(game_state.camera_free)
			camera.updateHorizontalAngle(deltaHorizontal);

		// set mouse pointer to the window center
		glutWarpPointer(game_state.windowWidth / 2, game_state.windowHeight / 2);

		glutPostRedisplay();
	}

	if (mouseY != game_state.windowHeight / 2 ) {

		float deltaVertical = (mouseY - game_state.windowHeight / 2) * 0.1f;
		if (game_state.camera_free)
			camera.updateVerticalAngle(deltaVertical);


		// set mouse pointer to the window center
		glutWarpPointer(game_state.windowWidth / 2, game_state.windowHeight / 2);

		glutPostRedisplay();
	}

}

void timerCb(int)
{
	game_state.elapsed_time = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); // milliseconds => seconds

	if (camera.inside)
	{
		camera.position = get_house_pos();
		camera.direction = get_house_dir();
		camera.center = camera.position + camera.direction;
		std::cerr << "cam dir :" << camera.direction[0] << " " << camera.direction[1] << " " << camera.direction[2] << std::endl;
	}
	else
		camera.update(game_state);

	lights.update(camera.position, camera.direction, game_state);
	
	game_state.smoke_current_time = game_state.elapsed_time;

	// and plan a new event
	glutTimerFunc(35, timerCb, 0);

	// create display event
	glutPostRedisplay();
}

