/*
 * penControl.h
 *
 *  Created on: 8 Oct 2020
 *      Author: Joel
 */

#ifndef PENCONTROL_H_
#define PENCONTROL_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

class penControl {
public:
	penControl(LPC_SCT_T * pSCT, CHIP_SWM_PIN_MOVABLE pSWM, int port, int pin);
	virtual ~penControl();
	void setPenPosition(uint8_t);
	uint8_t getPenPosition();
private:
	LPC_SCT_T * pSCT;
	float multiplier;
	uint8_t savedPenPos;			//Save pen position;
};



#endif /* PENCONTROL_H_ */
