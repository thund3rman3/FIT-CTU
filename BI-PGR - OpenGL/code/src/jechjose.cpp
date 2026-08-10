/**
 * @file    jechjose.cpp : This file contains the 'main' function.
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include <iostream>
#include "pgr.h"
#include "objects_render.h"
#include "cb.h"
#include "constants.h"

constexpr int WINDOW_WIDTH = 1680;
constexpr int WINDOW_HEIGHT = 850;
constexpr char WINDOW_TITLE[] = "PGR: jechjose";


/**
 * Initialize application data and OpenGL stuff.
 */
void initApplication() 
{
	glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearStencil(0);
	loadShaderPrograms();
	initializeModels();
	createMenu();
}

/**
 * Delete all OpenGL objects and application data.
 */
void finalizeApplication(void) 
{
	// delete geometry
	cleanupModels();

	//delete objects
	cleanUpObjects();

	// delete shaders
	cleanupShaderPrograms();
}


/**
 * Entry point of the application.
 * @param argc number of command line arguments
 * @param argv array with argument strings
 * @return 0 if OK, <> elsewhere
 */
int main(int argc, char** argv) 
{
	// initialize the GLUT library (windowing system)
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	// for each window
	{
		//   initial window size + title
		glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		glutCreateWindow(WINDOW_TITLE);

		// callbacks
		glutDisplayFunc(displayCb);					// draw scene
		glutReshapeFunc(reshapeCb);					// reshape window
		glutKeyboardFunc(keyboardCb);				// key pressed
		glutKeyboardUpFunc(keyboardUpCb);			// key released
		glutSpecialFunc(specialKeyboardCb);			// key pressed
		glutSpecialUpFunc(specialKeyboardUpCb);		// key released
		glutMouseFunc(mouseCb);						// mouse click
		glutPassiveMotionFunc(passiveMouseMotionCb);// mouse move
		glutTimerFunc(35, timerCb, 0);				// update
	}

	// initialize pgr-framework (GL, DevIl, etc.)
	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	// init shaders & program, buffers, locations, state of the application
	initApplication();

	// handle window close by the user
	glutCloseFunc(finalizeApplication);

	// Infinite loop handling the events
	glutMainLoop();

	return EXIT_SUCCESS;
}
