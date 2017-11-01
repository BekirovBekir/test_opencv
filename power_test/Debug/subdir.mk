################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ComPort.c \
../FrameBuffer.c \
../GPIO.c \
../ModemSARA.c \
../TempSensors.c \
../main.c 

OBJS += \
./ComPort.o \
./FrameBuffer.o \
./GPIO.o \
./ModemSARA.o \
./TempSensors.o \
./main.o 

C_DEPS += \
./ComPort.d \
./FrameBuffer.d \
./GPIO.d \
./ModemSARA.d \
./TempSensors.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


