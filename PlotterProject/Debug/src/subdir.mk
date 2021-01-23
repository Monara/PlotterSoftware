################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BinarySemaphore.cpp \
../src/DigitalIoPin.cpp \
../src/GCodeItem.cpp \
../src/PlotterController.cpp \
../src/RITStep.cpp \
../src/StepperController.cpp \
../src/cr_cpp_config.cpp \
../src/cr_startup_lpc15xx.cpp \
../src/main.cpp \
../src/penControl.cpp 

C_SRCS += \
../src/ITM_write.c \
../src/cdc_desc.c \
../src/cdc_main.c \
../src/cdc_vcom.c \
../src/crp.c \
../src/sysinit.c 

OBJS += \
./src/BinarySemaphore.o \
./src/DigitalIoPin.o \
./src/GCodeItem.o \
./src/ITM_write.o \
./src/PlotterController.o \
./src/RITStep.o \
./src/StepperController.o \
./src/cdc_desc.o \
./src/cdc_main.o \
./src/cdc_vcom.o \
./src/cr_cpp_config.o \
./src/cr_startup_lpc15xx.o \
./src/crp.o \
./src/main.o \
./src/penControl.o \
./src/sysinit.o 

CPP_DEPS += \
./src/BinarySemaphore.d \
./src/DigitalIoPin.d \
./src/GCodeItem.d \
./src/PlotterController.d \
./src/RITStep.d \
./src/StepperController.d \
./src/cr_cpp_config.d \
./src/cr_startup_lpc15xx.d \
./src/main.d \
./src/penControl.d 

C_DEPS += \
./src/ITM_write.d \
./src/cdc_desc.d \
./src/cdc_main.d \
./src/cdc_vcom.d \
./src/crp.d \
./src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -std=c++11 -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M3 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC15XX__ -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\lpc_board_nxp_lpcxpresso_1549\inc" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\lpc_chip_15xx\inc" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\FreeRTOS\inc" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\FreeRTOS\src\include" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\FreeRTOS\src\portable\GCC\ARM_CM3" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c11 -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M3 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC15XX__ -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\lpc_board_nxp_lpcxpresso_1549\inc" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\lpc_chip_15xx\inc" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\FreeRTOS\inc" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\FreeRTOS\src\include" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\FreeRTOS\src\portable\GCC\ARM_CM3" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\lpc_chip_15xx\inc\usbd" -I"C:\Users\David\Desktop\MCUxpresso Projects\workspace\PlotterProject\src" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


