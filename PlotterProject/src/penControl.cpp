/*
 * penControl.cpp
 *
 *  Created on: 8 Oct 2020
 *      Author: Joel
 */
#include "FreeRTOS.h"
#include "task.h"
#include "penControl.h"

penControl::penControl(LPC_SCT_T * pSCT, CHIP_SWM_PIN_MOVABLE pSWM, int port, int pin) {
	this->pSCT = pSCT;
	Chip_SCT_Init(pSCT);
	Chip_SCT_Init(LPC_SCT1);

	multiplier = float(1000) / (float)255;

	pSCT->CONFIG                    |= (1 << 17);               // Two 16-bit timers, auto limit
	pSCT->CTRL_L                    |= ((72-1) << 5);           // Set clock time to 72MHz/72=1MHz
	LPC_SCT1->CONFIG                |= (1 << 17);
	LPC_SCT1->CTRL_L                |= (1 << 17);

	pSCT->MATCHREL[0].L             = 20000-1;                  // Set the frequency 1MHz/20000 = 50Hz match frequency
	pSCT->MATCHREL[1].L             = 1000 + 160 * multiplier;  // Sets the duty cycle
	LPC_SCT1->MATCHREL[0].L         = 1000-1;
	LPC_SCT1->MATCHREL[1].L         = 1;

	pSCT->EVENT[0].STATE            = 0xFFFFFFFF;               // Event 0 happens in all states
	pSCT->EVENT[0].CTRL             = (1 << 12);                // Match 0 condition
	LPC_SCT1->EVENT[2].STATE        = 0xFFFFFFFF;
	LPC_SCT1->EVENT[2].CTRL         = (1 << 12);

	pSCT->EVENT[1].STATE            = 0xFFFFFFFF;               // Event 1 happens in all states
	pSCT->EVENT[1].CTRL             = (1 << 0) | (1 << 12);     // Match 1 condition
	LPC_SCT1->EVENT[3].STATE        = 0xFFFFFFFF;
	LPC_SCT1->EVENT[3].CTRL         = (1 << 0) | (1 << 12);

	pSCT->OUT[0].SET                = (1 << 0);                 // Event 0 sets current as high
	pSCT->OUT[0].CLR                = (1 << 1);                 // Event 1 sets current as low
	LPC_SCT1->OUT[1].SET            = 0;
	LPC_SCT1->OUT[1].CLR            = (1 << 1);

	Chip_SWM_MovablePortPinAssign(pSWM, port, pin);             // Assign SCT to Pen port and pin 0,10
	Chip_SWM_MovablePortPinAssign(SWM_SCT1_OUT1_O, 0, 12);

	pSCT->CTRL_L                    &= ~(1 << 2);               // Start the timer
	LPC_SCT1->CTRL_L                &= ~(1 << 2);
}

penControl::~penControl() {
	// TODO Auto-generated destructor stub
}

/*Set pen position, should be 0 - 255*/
void penControl::setPenPosition(uint8_t position){
	savedPenPos = position;
	pSCT->MATCHREL[1].L = 1000 + position * multiplier;         // Change duty cycle to change pen position
	vTaskDelay(100);
}

/*Return pen position, should be 0 - 255*/
uint8_t penControl::getPenPosition(){
	return savedPenPos;
}
