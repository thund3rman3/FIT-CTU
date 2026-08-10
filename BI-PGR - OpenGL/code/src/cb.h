/**
 * @file    cb.h
 * @author  Josef Jech
 * @date    15.05.2023
 */

#pragma once

// Function processing game menu
void gameMenu(int choice);

// Function generating menu
void createMenu(void);

//Draw the window contents.
void displayCb();

/**
 * Window was reshaped.
 * @param newWidth New window width
 * @param newHeight New window height
 */
void reshapeCb(int newWidth, int newHeight);

/**
 * Handle the key pressed event.
 * Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
 * parameter, which is an ASCII character. It's often a good idea to have the escape key (ASCII value 27)
 * to call glutLeaveMainLoop() to exit the program.
 * @param keyPressed ASCII code of the key
 * @param mouseX mouse (cursor) X position
 * @param mouseY mouse (cursor) Y position
 */
void keyboardCb(unsigned char keyPressed, int mouseX, int mouseY);

/**
* Called whenever a key on the keyboard was released. The key is given by
* the "keyReleased" parameter, which is in ASCII. 
* Handle the key released event.
* @param keyReleased ASCII code of the released key
* @param mouseX mouse (cursor) X position
* @param mouseY mouse (cursor) Y position
*/
void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY);

/**
* Handle the non-ASCII key pressed event (such as arrows or F1).
* The special keyboard callback is triggered when keyboard function (Fx) or directional
* keys are pressed.
* @param specKeyPressed int value of a predefined glut constant such as GLUT_KEY_UP
* @param mouseX mouse (cursor) X position
* @param mouseY mouse (cursor) Y position
*/
void specialKeyboardCb(int specKeyPressed, int mouseX, int mouseY);

/**
* Called whenever a key on the keyboard was released. The key is given by
* the "specKeyReleased" parameter, which is in non-ASCII.
* Handle the key released event.
* @param specKeyPressed int value of a predefined glut constant such as GLUT_KEY_UP
* @param mouseX mouse (cursor) X position
* @param mouseY mouse (cursor) Y position
*/
void specialKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY);

/**
* React to mouse button press and release (mouse click).
* When the user presses and releases mouse buttons in the window, each press
* and each release generates a mouse callback (including release after dragging).
*
* @param buttonPressed button code (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
* @param buttonState GLUT_DOWN when pressed, GLUT_UP when released
* @param mouseX mouse (cursor) X position
* @param mouseY mouse (cursor) Y position
*/
void mouseCb(int buttonPressed, int buttonState, int mouseX, int mouseY);

/**
* Handle mouse movement over the window (with no button pressed).
* @param mouseX mouse (cursor) X position
* @param mouseY mouse (cursor) Y position
*/
void passiveMouseMotionCb(int mouseX, int mouseY);

/**
* Callback responsible for the scene update.
*/
void timerCb(int);
