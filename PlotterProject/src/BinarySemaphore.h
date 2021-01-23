/*
 * BinarySemaphore.h
 *
 *  Created on: 28 Aug 2020
 *      Author: Asus
 */

#ifndef BINARYSEMAPHORE_H_
#define BINARYSEMAPHORE_H_

#include "FreeRTOS.h"
#include "semphr.h"


class BinarySemaphore {

public:
	BinarySemaphore();
	virtual ~BinarySemaphore();
	void give();
	void giveFromISR(BaseType_t *pxHigherPriorityTaskWoken);
	bool take();
private:
	SemaphoreHandle_t semaphore;

};



#endif /* BINARYSEMAPHORE_H_ */
