/*
 * motors.c
 *
 *  Created on: Feb 1, 2017
 *      Author: Tom
 */
#include <RBELib/RBELib.h>
#include <avr/io.h>

/**
 * @brief Helper function to stop the motors on the arm.
 *
 * @todo Create way to stop the motors using the DAC.
 */
void stopMotors() {
	setDAC(0, 0);
	setDAC(1, 0);
	setDAC(2, 0);
	setDAC(3, 0);
}

/**
 * @brief Drive the arm to a desired angle
 *
 * @param lowerTheta The desired angle for the lower link.
 * @param upperTheta The desired angle for the upper link.
 *
 * @todo Make a way to drive the links to a desired angle.
 */
void gotoAngles(int lowerTheta, int upperTheta) {

}

/**
 * @brief Drive the end effector of the arm to a desired X and Y position in the workspace.
 *
 * @param x The desired x position for the end effector.
 * @param y The desired y position for the end effector.
 *
 * @todo Use kinematic equations to move the end effector to the desired position.
 */
void gotoXY(int x, int y) {

}

/**
 * @brief Drive a link (upper or lower) in a desired direction.
 *
 * @param link Which link to control.
 * @param dir Which way to drive the link.
 *
 * @todo Create a way to drive either link in any direction.
 */
void driveLink(char link, int value) {
	if (value > 4095) {
		value = 4095;
	} else if (value < -4095) {
		value = -4095;
	}

	switch (link) {
	case 'H':
		if (value >= 0) {
			setDAC(2, value);
			setDAC(3, 0);
		} else {
			setDAC(2, 0);
			setDAC(3, value);
		}
		break;
	}
	case 'L':
	if (value >= 0) {
		setDAC(0, 0);
		setDAC(1, value);
	} else {
		setDAC(0, value);
		setDAC(1, 0);
	}

	/**
	 * @brief Drive the arm to a "home" position using the potentiometers.  This
	 * should be called before using the encoders and just goes to a default position.
	 * Once this has been called once, you can initialize/clear the encoders.
	 *
	 * @todo Drive the arm to a known position using the potentiometers.
	 */
	void homePos() {

	}
