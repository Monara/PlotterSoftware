/*

Author: Monika
Embedded System Project: Gcode Parser
September 2020

-	GCodeItem class based on commands listed in XY-plotter_specifications.pdf
-	Enum should be useful in implementing plotter behaviour, instead of using the same non-descriptive command codes
-	parse function parses a line and outputs a successfully gathered command with possible parameters into GCodeItem object

*/
#include "GCodeItem.h"
#include <cstring> //strcmp()
#include <cstdio> //sscanf()

bool GCodeItem::parse(const char* line, GCodeItem& output) {

	output = GCodeItem(); //clear object that gets reused

	if (strncmp("G1 ", line, 3) == 0) { //most popular command first. Format: G1 X104.43 Y51.75 A0
		
		float xCoordinate, yCoordinate;
		int relOrAbs = 0;

		if (sscanf(line + 3, "X%f Y%f A%d", &xCoordinate, &yCoordinate, &relOrAbs) == 3) {

			if (relOrAbs == 0 || relOrAbs == 1) { 

				output.command = GCodeItem::GO_TO_POSITION;
				output.xCoordinate = xCoordinate;
				output.yCoordinate = yCoordinate;
				output.isRelativeCoordinate = (bool)relOrAbs;
				return true;
			}
		}
	}
	else if (strcmp("G28", line) == 0) { //G28 has no parameters, checking all line
		output.command = GCodeItem::GO_TO_ORIGIN; //G28
		return true;
	}
	
	else if (strncmp("M1 ", line, 3) == 0) {

		int penPosition = 0;

		if (sscanf(line + 3, "%d", &penPosition) == 1) {
			if (penPosition >= 0 && penPosition <= 255) {
				output.command = GCodeItem::SET_PEN_POSITION; //M1. Format: M1 90
				output.penPosition = penPosition;
				return true; //too many "else return false" otherwise
			}
		}
	}
	else if (strncmp("M2 ", line, 3) == 0) {

		int penUp, penDown;

		if (sscanf(line + 3, "U%d D%d", &penUp, &penDown) == 2) { //only assign members if values found
			output.command = GCodeItem::SAVE_PEN_POSITION; //M2. Format: M2 U150 D90
			output.penUp = penUp;
			output.penDown = penDown;
			return true;
		}
	}
	else if (strncmp("M4 ", line, 3) == 0) {
		
		int laserPower = 0;
		if (sscanf(line + 3, "%d", &laserPower) == 1) {
			if (laserPower >= 0 && laserPower <= 255) {
				output.command = GCodeItem::SET_LASER_POWER; //M4. Format: M4 90
				output.laserPower = laserPower;
				return true;
			}
		}
	}
	else if (strncmp("M5 ", line, 3) == 0) {

		int motorA, motorB, areaHeight, areaWidth, plottingSpeed;

		if (sscanf(line + 3, "A%d B%d H%d W%d S%d", &motorA, &motorB, &areaHeight, &areaWidth, &plottingSpeed) == 5) {
			if ((motorA == 0 || motorA == 1) && (motorB == 0 || motorB == 1)) { //both have to be bool
			
				output.command = GCodeItem::SAVE_PLOTTING_PARAMS; //M5. Format: M5 A0 B0 H310 W380 S80
				output.isStepperXReverse = (bool)motorA;
				output.isStepperYReverse = (bool)motorB;
				output.areaHeight = areaHeight;
				output.areaWidth = areaWidth;
				output.plottingSpeed = plottingSpeed;
				return true;
			}
		}
	}
	else if (strcmp("M10", line) == 0) {  //M10 has no parameters, checking all line
		output.command = GCodeItem::OPEN_PORT;
		return true;
	}
	else if (strcmp("M11", line) == 0) { //M11 has no parameters
		output.command = GCodeItem::LIMIT_QUERY; //M11
		return true;
	}

	return false;//NO_COMMAND assigned by default
}
