#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "GCodeItem.h"
#include "user_vcom.h" //CDC task declaration
#include "ITM_write.h"
#include "PlotterController.h"
#include "RITStep.h"
#include <cstring>
#include "BinarySemaphore.h"

BinarySemaphore resetSemaphore;                //Semaphore for reset button
/* Sets up system hardware etc. */
static void setup() {

	SystemCoreClockUpdate();
	Board_Init();
	Board_LED_Set(0, false);
	RITStepInit();
	ITM_init(); //prints if USB connected

}

extern "C" {
/*interrupt handler for reset button*/
void PIN_INT4_IRQHandler(){
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH4);
	BaseType_t xHigherPriorityWoken = pdFALSE;
	resetSemaphore.giveFromISR(&xHigherPriorityWoken);
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

}

static void vTask1(void *pvParameters) {
	/* setup reset button interrupt*/
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	NVIC_SetPriority( PIN_INT4_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_ClearPendingIRQ(PIN_INT4_IRQn);

	DigitalIoPin sw1 = DigitalIoPin(0, 8, DigitalIoPin::pullup, true);

	Chip_INMUX_PinIntSel(4, 0, 8);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH4);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH4);

	vTaskDelay(100 / portTICK_PERIOD_MS);

	PlotterController plotterController;

	vTaskDelay(100 / portTICK_PERIOD_MS);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH4);
	NVIC_EnableIRQ(PIN_INT4_IRQn);

	while (plotterController.anyLimitsClosed()) {               //Wait for all limits switches to be open
		vTaskDelay(1);
	}

	plotterController.findBounds();                            //Find bounds and calculate amount of steps between them
	plotterController.findMaxSpeed();                          //Find max speed stepper motor can operate on

	const int bufferSize = 50;

	char readBuffer[bufferSize];
	char commandBuffer[bufferSize];

	int commandLength = 0;

	USB_wait_for_connect(); //added to cdc_main.c

	while(1) {

		int count = USB_receive((uint8_t*)readBuffer, bufferSize);

		for (int i = 0; i < count; i++) {

			if (commandLength > bufferSize - 2 || readBuffer[i] == '\n' || readBuffer[i] == '\r') { //if buffer full or newline

				commandBuffer[commandLength] = '\0'; //not to parse garbage
				GCodeItem item;
				commandLength = 0;

				if (GCodeItem::parse(commandBuffer, item)) {


					if (item.command == GCodeItem::SAVE_PEN_POSITION) { //M2

						plotterController.setPenUpVal(item.penUp);
						plotterController.setPenDownVal(item.penDown);
					}

					else if (item.command == GCodeItem::SAVE_PLOTTING_PARAMS) { //M5

						plotterController.setAreaWidth(item.areaWidth);
						plotterController.setAreaHeight(item.areaHeight);
						plotterController.setStepperXDir(item.isStepperXReverse);
						plotterController.setStepperYDir(item.isStepperYReverse);
						plotterController.setPlottingSpeedPercent(item.plottingSpeed);

					}

					else if (item.command == GCodeItem::OPEN_PORT) { //M10

						char printBuffer[bufferSize];

						//example M10 XY 380 310 0.00 0.00 A0 B0 H0 S80 U160 D90<CR><LF>.

						plotterController.stepMultiplier(plotterController.getAreaWidth(), plotterController.getAreaHeight());

						int xDir = plotterController.getStepperXDir() ? 1 : 0; //bool to int
						int yDir = plotterController.getStepperYDir() ? 1 : 0;

						sprintf(printBuffer, "M10 XY %d %d 0.00 0.00 A%d B%d H0 S%d U%d D%d\r\n",
								plotterController.getAreaWidth(), plotterController.getAreaHeight(),
								xDir, yDir, plotterController.getPlottingSpeedPercent(), plotterController.getPenUpVal(), plotterController.getPenDownVal());

						USB_send((uint8_t*) printBuffer, strlen(printBuffer));
					}

					else if (item.command == GCodeItem::LIMIT_QUERY) { //M11

						char printBuffer[bufferSize];

						//example M11 1 1 1 1<CR><LF>
						int lim1 = plotterController.readLimitRight() ? 0 : 1; //bool to ints
						int lim2 = plotterController.readLimitLeft() ? 0: 1;
						int lim3 = plotterController.readLimitTop() ? 0: 1;
						int lim4 = plotterController.readLimitBottom() ? 0 : 1;

						sprintf(printBuffer, "M11 %d %d %d %d\r\n", lim1, lim2, lim3, lim4); //not sure about printing order
						USB_send((uint8_t*) printBuffer, strlen(printBuffer));
					}

					else if (item.command == GCodeItem::GO_TO_POSITION){ //G1

						plotterController.drawLine(item.xCoordinate, item.yCoordinate);

					}

					else if (item.command == GCodeItem::GO_TO_ORIGIN){ //G28
						plotterController.setPenPosition(plotterController.getPenUpVal()); //Set pen up so we don't draw on the way to ORIGIN
						plotterController.drawLine(0, 0);

					}

					else if (item.command == GCodeItem::SET_PEN_POSITION){//M1

						plotterController.setPenPosition(item.penPosition);

					}

					USB_send((uint8_t*)"OK\r\n", 4); //all finally send ok
				}
			}

			if (readBuffer[i] != '\n' && readBuffer[i] != '\r') { // (not else if: not to get rid of char with full buffer)

				commandBuffer[commandLength] = readBuffer[i]; //written into command buffer
				commandLength++; //increment position marker

			}

			if(plotterController.anyLimitsClosed()){
				plotterController.setPenPosition(plotterController.getPenUpVal());		//Set pen up
				resetSemaphore.take();														//Wait for limit reset button SW1
				plotterController.findBounds();						                  //Find bounds again
				char tempBuffer[256];
				USB_receive((uint8_t*)tempBuffer, 256); 			                  //Empty the USB
				commandLength = 0;
				break;
			}
		}
	}
}


int main() {

	setup();

	xTaskCreate(cdc_task, "CDC",
					configMINIMAL_STACK_SIZE * 3, NULL, (tskIDLE_PRIORITY + 1UL),
					(TaskHandle_t *) NULL);

	xTaskCreate(vTask1, "Task1",
						configMINIMAL_STACK_SIZE * 7, NULL, (tskIDLE_PRIORITY + 1UL),
						(TaskHandle_t *) NULL);

	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
