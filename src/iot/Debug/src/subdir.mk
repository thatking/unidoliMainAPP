################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/iot_test.c \
../src/mqtt-example.c 

OBJS += \
./src/iot_test.o \
./src/mqtt-example.o 

C_DEPS += \
./src/iot_test.d \
./src/mqtt-example.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-hisiv500-linux-gcc -I/home/hp/Public/eclipse_workspace/iot_test/include -I/home/hp/Public/eclipse_workspace/iot_test/include/iot-sdk -I/home/hp/Public/eclipse_workspace/iot_test/include/mbedtls -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


