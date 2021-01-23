/*
 * StepperMotor.h
 *
 *  Created on: 13 Oct 2020
 *      Author: Monika
 */

#ifndef STEPPERMOTORCONTROLLER_H_
#define STEPPERMOTORCONTROLLER_H_

#include "DigitalIoPin.h"

class StepperController {

public:
	StepperController();

	void setStepperMotorX(bool val);
	void setStepperMotorY(bool val);

	void setMotorXDirection(bool val);
	void setMotorYDirection(bool val);

	bool readLimitRight();
	bool readLimitLeft();
	bool readLimitTop();
	bool readLimitBottom();

private:

	DigitalIoPin stepperMotorX;
	DigitalIoPin stepperMotorY;
	DigitalIoPin motorXDirection;
	DigitalIoPin motorYDirection;

	DigitalIoPin limitRight;
	DigitalIoPin limitLeft;
	DigitalIoPin limitTop;
	DigitalIoPin limitBottom;

};



#endif /* STEPPERMOTORCONTROLLER_H_ */
