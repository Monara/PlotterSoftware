/*
 * PlotterController.cpp
 *
 *  Created on: 13 Oct 2020
 *      Author: Monika
 *
D6 1_3 Limit SW
D7 0_0 Limit SW
D3 0_9 Limit SW
D2 0_29 Limit SW
D12 0_12 Laser
D4 0_10 Pen
D10 0_24 XMotor
D11 1_0 Xmotor Direction
D8 0_27 YMotor
D9 0_28 YMotor Direction
A0 0_8 SW1
A1 1_6 SW2
A2 1_8 SW3
 */

#include "PlotterController.h"
#include "RITStep.h"


PlotterController::PlotterController() :
		pen(LPC_SCT0, SWM_SCT0_OUT0_O, 0, 10)
{
	initLimitInterrupts();
}

void PlotterController::findBounds() { //no speed ramp up here possible, only 1 step at a time

	int stepSumWidth = 0;
	int stepSumHeight = 0;

	disableLimitInterrupts(); //Disable interrupts so they don't interfere

	//Cannot count from unknown position. Find left limit:

	while (stepperController.readLimitLeft() == false) { //go towards left limit

		RITStep(-1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
	}

	while (stepperController.readLimitLeft() == true) { //reverse until limit switch released

		RITStep(1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
	}

	//Width: step counting

	while (stepperController.readLimitRight() == false) { //go towards right limit. Start counting

		RITStep(1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumWidth++;
	}

	while (stepperController.readLimitRight() == true) { //reverse until limit switch released, minus steps

		RITStep(-1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumWidth--;
	}

	while (stepperController.readLimitLeft() == false) { //go towards left limit. Continue counting

		RITStep(-1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumWidth++;
	}

	while (stepperController.readLimitLeft() == true) { //reverse until limit switch released, minus steps

		RITStep(1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumWidth--;
	}

	this->areaWidth = stepSumWidth / 2; //average of steps before hitting limits


	//Height. Cannot count from unknown position. Find bottom limit:

	while (stepperController.readLimitBottom() == false) { //go towards bottom limit

		RITStep(0, -1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
	}

	while (stepperController.readLimitBottom() == true) { //reverse until limit switch released

		RITStep(0, 1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
	}

	//Height: step counting
	while (stepperController.readLimitTop() == false) { //go towards top limit. Start counting

		RITStep(0, 1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumHeight++;
	}

	while (stepperController.readLimitTop() == true) { //reverse until limit switch released, minus steps

		RITStep(0, -1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumHeight--;
	}

	while (stepperController.readLimitBottom() == false) { //go towards Bottom limit. Continue counting

		RITStep(0, -1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumHeight++;
	}

	while (stepperController.readLimitBottom() == true) { //reverse until limit switch released, minus steps

		RITStep(0, 1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
		stepSumHeight--;
		RITStep(0, 1, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
	}

	this->areaHeight = stepSumHeight / 2; //average of steps before hitting limits

	enableLimitInterrupts();
	xStart = 0;
	yStart = 0;
}

void PlotterController::findMaxSpeed() {

	disableLimitInterrupts();

	int targetDelayUs = this->initialPulseWidthUs;
	int lastSuccessfulPulseWidth = this->initialPulseWidthUs;
	bool runRight = true;

	while(1) {

		// run left or right to switch
		if (runRight) {

			RITStep(this->areaWidth+1, 0, this->initialPulseWidthUs, targetDelayUs, &stepperController);
		}

		else {

			RITStep(-this->areaWidth-1, 0, this->initialPulseWidthUs, targetDelayUs, &stepperController);
		}

		// check if switch is pressed, if so increase speed and reverse direction
		if (anyLimitsClosed()) {

			while(anyLimitsClosed()) { //back up from switch

				if(runRight) {

					RITStep(-1, 0, this->initialPulseWidthUs, targetDelayUs, &stepperController);
				}

				else {

					RITStep(1, 0, this->initialPulseWidthUs, targetDelayUs, &stepperController);
				}
			}

			lastSuccessfulPulseWidth = targetDelayUs;
			targetDelayUs -= targetDelayUs / 10; //decrease pulse width by 10 %
			runRight = !runRight;
		}

		else { //if limit switch wasn't hit, running too fast

			break;
		}
	}

	this->targetPulseWidthUs = lastSuccessfulPulseWidth; //pulse width value for max speed found

	while (stepperController.readLimitLeft() == false) { //go towards left limit from unknown position

		RITStep(-1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController); //already with speed ramp
	}

	while (stepperController.readLimitLeft() == true) { //reverse until limit switch released, minus steps

		RITStep(1, 0, this->initialPulseWidthUs, this->targetPulseWidthUs, &stepperController);
	}

	enableLimitInterrupts();  //Enable limit interrupts again for plotting
	xStart = 0;
	yStart = 0;

}


void PlotterController::setAreaWidth(int width) {
	this->areaWidth = width;
}

void PlotterController::setAreaHeight(int height) {
	this->areaHeight = height;
}

int PlotterController::getAreaWidth() const {
	return this->areaWidth;
}

int PlotterController::getAreaHeight() const {
	return this->areaHeight;
}

void PlotterController::setStepperXDir(bool xDir) {
	this->stepperXDir = xDir;
}

void PlotterController::setStepperYDir(bool yDir) {
	this->stepperYDir = yDir;
}

bool PlotterController::getStepperXDir() const {
	return this->stepperXDir;
}

bool PlotterController::getStepperYDir() const {
	return this->stepperYDir;
}

void PlotterController::setPlottingSpeedPercent(int speed) {
	this->plottingSpeedPercent = speed;
}

int PlotterController::getPlottingSpeedPercent() const {
	return this->plottingSpeedPercent;
}

void PlotterController::setPenUpVal(int upVal) {
	this->penUpVal = upVal;
}

void PlotterController::setPenDownVal(int downVal) {
	this->penDownVal = downVal;
}

int PlotterController::getPenUpVal() const {
	return this->penUpVal;
}

int PlotterController::getPenDownVal() const {
	return this->penDownVal;
}

bool PlotterController::anyLimitsClosed() {
	return stepperController.readLimitRight()
			|| stepperController.readLimitLeft()
			|| stepperController.readLimitTop()
			|| stepperController.readLimitBottom();
}

bool PlotterController::readLimitRight() {
	return stepperController.readLimitRight();
}
bool PlotterController::readLimitLeft() {
	return stepperController.readLimitLeft();
}
bool PlotterController::readLimitTop() {
	return stepperController.readLimitTop();
}
bool PlotterController::readLimitBottom() {
	return stepperController.readLimitBottom();
}
/*function receives XY coordinates, calculates the change in steps and starts plotting*/
void PlotterController::drawLine(float x, float y) {
	int xEnd = (x * 10000) / xMultiplier;         //Use multipliers to make x and y coordinates more accurate
	int yEnd = (y * 10000) / yMultiplier;

	int targetDelayUs = (this->plottingSpeedPercent * (this->targetPulseWidthUs - this->initialPulseWidthUs)) / 100 + this->initialPulseWidthUs; //speed depends of plotting speed in percent

	int dx = xEnd - xStart;     //Amount of xSteps
	int dy = yEnd - yStart;	    //Amount of ySteps

	RITStep(dx, dy,  this->initialPulseWidthUs, targetDelayUs, &stepperController);

	xStart = xEnd;                      //Set new starting coordinates for next plot segment
	yStart = yEnd;
}
/*Get step multipliers for increased accuracy instead of using floats for coordinates*/
void PlotterController::stepMultiplier(float width, float height){
	xMultiplier = (width * 10000) / areaWidth;
	yMultiplier = (height * 10000) / areaHeight;
}

void PlotterController::setPenPosition(int a){
	pen.setPenPosition(a);
}
