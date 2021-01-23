/*
 * RITStep.cpp
 *
 *  Created on: 13 Oct 2020
 *      Author: Monika
 */

#include "RITStep.h"
#include "BinarySemaphore.h"
#include "StepperController.h"
#include <cmath> //for abs()

static BinarySemaphore semaphoreRIT;
static int xStepsRIT = 0;
static int yStepsRIT = 0;
static StepperController* stepperControllerRIT = NULL;
static bool pulseHigh = false; //for toggling between high and low

//variables for utilizing max speed
static int stepsTaken = 0;
static int rampUpEnd = 0;
static int rampDownStart = 0;
static int currentPulseWidth = 0;
static int targetPulseWidth = 0;
static int pulseWidthChange = 0;

void RITStepInit() {
	// initialize RIT (= enable clocking etc.)
	Chip_RIT_Init(LPC_RITIMER);
	// set the priority level of the interrupt
	// The level must be equal or lower than the maximum priority specified in FreeRTOS config
	// Note that in a Cortex-M3 a higher number indicates lower interrupt priority
	NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2 );
}

extern "C" {

void RIT_IRQHandler(void) {

	portBASE_TYPE xHigherPriorityWoken = pdFALSE; // This used to check if a context switch is required
	Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag

	if (pulseHigh) {

		pulseHigh = false;
		stepperControllerRIT->setStepperMotorX(false); //write low to pin
		stepperControllerRIT->setStepperMotorY(false);


		stepsTaken++;

		if (stepsTaken < rampUpEnd) {

			//speeding up
			currentPulseWidth += pulseWidthChange;
		}

		else if (stepsTaken > rampDownStart) {

			//slowing down
			currentPulseWidth -= pulseWidthChange;
		}

		else {

			//speed is target speed
			currentPulseWidth = targetPulseWidth;
		}

		uint64_t cmp_value = ((uint64_t)Chip_Clock_GetSystemClockRate() * (uint64_t) currentPulseWidth) / 1000000;
		Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);

	}

	else { // if pulse is low

		if (xStepsRIT == 0 && yStepsRIT == 0) {

			//target reached, end routine
			Chip_RIT_Disable(LPC_RITIMER); // disable timer
			semaphoreRIT.giveFromISR(&xHigherPriorityWoken); // Give semaphore and set context switch flag if a higher priority task was woken up

		}

		else {

			pulseHigh = true;


			if ((xStepsRIT > yStepsRIT) && (xStepsRIT > 0)){               //Check if x coordinate change is larger than y

				int D = 2*yStepsRIT - xStepsRIT;
				stepperControllerRIT->setStepperMotorX(true);

				if (D > 0){
					stepperControllerRIT->setStepperMotorY(true);
					yStepsRIT--;
				}

				xStepsRIT--;

			}else if ((xStepsRIT < yStepsRIT) && (yStepsRIT > 0)){               //Check if y coordinate change is larger than x

				int D = 2*xStepsRIT - yStepsRIT;
				stepperControllerRIT->setStepperMotorY(true);

				if (D > 0){
					stepperControllerRIT->setStepperMotorX(true);
					xStepsRIT--;
				}

				yStepsRIT--;

			}else if((xStepsRIT == yStepsRIT) && ((xStepsRIT && yStepsRIT) != 0)){         //if x and y change is the same
				stepperControllerRIT->setStepperMotorX(true);
				stepperControllerRIT->setStepperMotorY(true);
				xStepsRIT--;
				yStepsRIT--;
			}
		}


	}

	portEND_SWITCHING_ISR(xHigherPriorityWoken); // End the ISR and (possibly) do a context switch

}

}

void RITStep(int xSteps, int ySteps, int initialPulseWidthUs, int targetPulseWidthUs, StepperController *stepper) {

	// for RIT IRQ handler
	xStepsRIT = abs(xSteps); //take absolute val, direction set below
	yStepsRIT = abs(ySteps);
	stepperControllerRIT = stepper;
	pulseHigh = false;

	int totalSteps = xStepsRIT > yStepsRIT ? xStepsRIT : yStepsRIT; // whichever axis is longer will be used for speed ramping
	rampUpEnd = totalSteps / 5; //20 % for ramping up and down;
	rampDownStart = totalSteps - rampUpEnd;
	currentPulseWidth = initialPulseWidthUs;
	targetPulseWidth = targetPulseWidthUs;
	pulseWidthChange = (targetPulseWidth - currentPulseWidth) / rampUpEnd; //pulse width change per ramp step
	stepsTaken = 0;

	stepper->setMotorXDirection(xSteps > 0); //set direction. Negative val: reverse (true), positive val: forwards (false)
	stepper->setMotorYDirection(ySteps > 0);

	uint64_t cmp_value = ((uint64_t) Chip_Clock_GetSystemClockRate() * (uint64_t) initialPulseWidthUs) / 1000000;	// Determine approximate compare value based on clock rate and passed interval
	Chip_RIT_Disable(LPC_RITIMER);	// disable timer during configuration
	Chip_RIT_EnableCompClear(LPC_RITIMER); // enable automatic clear on when compare value==timer value
	Chip_RIT_SetCounter(LPC_RITIMER, 0); // reset the counter
	Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
	Chip_RIT_Enable(LPC_RITIMER); // start counting
	NVIC_EnableIRQ(RITIMER_IRQn); // Enable the interrupt signal in NVIC (the interrupt controller)
	 // wait for ISR to tell that we're done

	if(semaphoreRIT.take()) {
		NVIC_DisableIRQ(RITIMER_IRQn); // Disable the interrupt signal in NVIC (the interrupt controller)
	}
	 else {
	 // unexpected error
	 }

	// Reset variables
	stepperControllerRIT = NULL;
	xStepsRIT = 0;
	yStepsRIT = 0;
	pulseHigh = false;

}

/*Handlers for limit interrupts*/
extern "C" {

void PIN_INT0_IRQHandler(void){
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
	BaseType_t xHigherPriorityWoken = pdFALSE;
	xStepsRIT = 0;                 //Set steps to 0 so RIT knows to stop
	yStepsRIT = 0;
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

void PIN_INT1_IRQHandler(void){
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
	BaseType_t xHigherPriorityWoken = pdFALSE;
	xStepsRIT = 0;                 //Set steps to 0 so RIT knows to stop
	yStepsRIT = 0;
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

void PIN_INT2_IRQHandler(void){
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
	BaseType_t xHigherPriorityWoken = pdFALSE;
	xStepsRIT = 0;                 //Set steps to 0 so RIT knows to stop
	yStepsRIT = 0;
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

void PIN_INT3_IRQHandler(void){
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
	BaseType_t xHigherPriorityWoken = pdFALSE;
	xStepsRIT = 0;                 //Set steps to 0 so RIT knows to stop
	yStepsRIT = 0;
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

}
/*Initialize limit switch pin interrupts*/
void initLimitInterrupts(){
	NVIC_SetPriority( PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority( PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority( PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority( PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

	Chip_INMUX_PinIntSel(0, 0, 29);
	Chip_INMUX_PinIntSel(1, 0, 9);
	Chip_INMUX_PinIntSel(2, 1, 3);
	Chip_INMUX_PinIntSel(3, 0, 0);

	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH1);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH2);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH3);

	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH1);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH2);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH3);

}

void disableLimitInterrupts(){
	NVIC_DisableIRQ(PIN_INT0_IRQn);
	NVIC_DisableIRQ(PIN_INT1_IRQn);
	NVIC_DisableIRQ(PIN_INT2_IRQn);
	NVIC_DisableIRQ(PIN_INT3_IRQn);
}

void enableLimitInterrupts(){
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT3_IRQn);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);

	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT3_IRQn);
}

