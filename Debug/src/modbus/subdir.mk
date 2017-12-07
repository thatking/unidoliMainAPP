################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modbus/modbus.c \
../src/modbus/modbus_app.c \
../src/modbus/serial.c 

OBJS += \
./src/modbus/modbus.o \
./src/modbus/modbus_app.o \
./src/modbus/serial.o 

C_DEPS += \
./src/modbus/modbus.d \
./src/modbus/modbus_app.d \
./src/modbus/serial.d 


# Each subdirectory must supply rules for building sources it contributes
src/modbus/%.o: ../src/modbus/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-hisiv500-linux-gcc -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libcurl4.4.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/iot-sdk -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/mbedtls -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/modbus -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libunidoli_rtmp -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/libhisi1.0.1.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/wpa_supplicant -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


