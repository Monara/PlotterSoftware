/*
 * RITStep.h
 *
 *  Created on: 13 Oct 2020
 *      Author: Monika
 */

#ifndef RITSTEP_H_
#define RITSTEP_H_

class StepperController; //forward declaration for function below

void RITStepInit();
void RITStep(int xSteps, int ySteps, int initialPulseWidthUs, int targetPulseWidthUs, StepperController *stepper); //cycle in microseconds
void initLimitInterrupts();
void disableLimitInterrupts();
void enableLimitInterrupts();



#endif /* RITSTEP_H_ */
