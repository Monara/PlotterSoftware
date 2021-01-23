/*
 * PlotterController.h
 *
 *  Created on: 13 Oct 2020
 *      Author: Monika
 */

#ifndef PLOTTERCONTROLLER_H_
#define PLOTTERCONTROLLER_H_

#include "DigitalIoPin.h"
#include "StepperController.h"
#include "penControl.h"



class PlotterController {

public:
	PlotterController();

	void findBounds();
	void findMaxSpeed();

	void setAreaWidth(int width);
	void setAreaHeight(int height);
	int getAreaWidth() const;
	int getAreaHeight() const;

	void setStepperXDir(bool xDir);
	void setStepperYDir(bool yDir);
	bool getStepperXDir() const;
	bool getStepperYDir() const;

	void setPlottingSpeedPercent(int speed);
	int getPlottingSpeedPercent() const;

	void setPenUpVal(int upVal);
	void setPenDownVal(int downVal);
	int getPenUpVal() const;
	int getPenDownVal() const;

	bool anyLimitsClosed();
	bool readLimitRight();
	bool readLimitLeft();
	bool readLimitTop();
	bool readLimitBottom();

	void drawLine(float x, float y);
	void stepMultiplier(float xFrame, float yFrame);
	void setPenPosition(int a);

	int xMultiplier = 0;
	int yMultiplier = 0;

private:

	StepperController stepperController;
	penControl pen;

	int areaWidth = 0;
	int areaHeight = 0;
	bool stepperXDir = false;
	bool stepperYDir = false;
	int plottingSpeedPercent = 0;
	int penUpVal = 160;
	int penDownVal = 90;
	int initialPulseWidthUs = 1000; //2000 microsec cycle
	int targetPulseWidthUs = 1000; //init to same as initial

	int xStart = 0;
	int yStart = 0;
};


#endif /* PLOTTERCONTROLLER_H_ */
