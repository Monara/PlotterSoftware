/*
 * GCodeItem.h
 *
 *  Created on: 8 Oct 2020
 *      Author: Monika
 */

#ifndef GCODEITEM_H_
#define GCODEITEM_H_

class GCodeItem {
public:

	enum CommandType {
		GO_TO_POSITION, //G1
		GO_TO_ORIGIN, //G28
		SET_PEN_POSITION, //M1
		SAVE_PEN_POSITION, //M2
		SET_LASER_POWER, //M4
		SAVE_PLOTTING_PARAMS, //M5
		OPEN_PORT, //M10
		LIMIT_QUERY, //M11
		NO_COMMAND //for junk
	};

	CommandType command = NO_COMMAND;

	float xCoordinate = 0.0; //for G1
	float yCoordinate = 0.0; //for G1
	bool isRelativeCoordinate = false; // 0 absolute, 1 relative. for G1
	int penPosition = 0; //between 0-255 for M1
	int penUp = 0; //for M2
	int penDown = 0;//for M2
	int laserPower = 0; //between 0-255 for M4
	bool isStepperXReverse = false; //for M5
	bool isStepperYReverse = false; //for M5
	int areaHeight = 0; //for M5
	int areaWidth = 0; //for M5
	int plottingSpeed = 0; //for M5

	static bool parse(const char* line, GCodeItem& output);

};


#endif /* GCODEITEM_H_ */
