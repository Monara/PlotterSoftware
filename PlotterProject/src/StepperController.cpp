/*
 * StepperMotor.cpp
 *
 *  Created on: 13 Oct 2020
 *      Author: Monika
 */

#include "StepperController.h"

StepperController::StepperController() :
		stepperMotorX(0, 24, DigitalIoPin::output, false),
		stepperMotorY(0, 27, DigitalIoPin::output, false),
		motorXDirection(1, 0, DigitalIoPin::output, false),
		motorYDirection(0, 28, DigitalIoPin::output, false),
		limitRight(0, 29, DigitalIoPin::pullup, true),
		limitLeft(0, 9, DigitalIoPin::pullup, true),
		limitTop(1, 3, DigitalIoPin::pullup, true ),
		limitBottom(0, 0, DigitalIoPin::pullup, true)
{

}

void StepperController::setStepperMotorX(bool val) {
	stepperMotorX.write(val);
}
void StepperController::setStepperMotorY(bool val) {
	stepperMotorY.write(val);
}

void StepperController::setMotorXDirection(bool val) {
	motorXDirection.write(val);
}
void StepperController::setMotorYDirection(bool val) {
	motorYDirection.write(val);
}

bool StepperController::readLimitRight() {
	return limitRight.read();
}

bool StepperController::readLimitLeft() {
	return limitLeft.read();
}

bool StepperController::readLimitTop() {
	return limitTop.read();
}

bool StepperController::readLimitBottom() {
	return limitBottom.read();
}
