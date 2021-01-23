/*
 * BinarySemaphore.cpp
 *
 *  Created on: 28 Aug 2020
 *      Author: Asus
 */

#include "BinarySemaphore.h"

BinarySemaphore::BinarySemaphore() {

	semaphore = xSemaphoreCreateBinary();
}

BinarySemaphore::~BinarySemaphore() {

}

void BinarySemaphore::give() {

	xSemaphoreGive(semaphore);
}

void BinarySemaphore::giveFromISR(BaseType_t *pxHigherPriorityTaskWoken) {

	xSemaphoreGiveFromISR(semaphore, pxHigherPriorityTaskWoken);
}

bool BinarySemaphore::take() {

	return xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE; //returns true or false
}



