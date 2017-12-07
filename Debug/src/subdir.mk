################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/curlGetDat.c \
../src/iot.c \
../src/network.c \
../src/rtmp.c \
../src/types.c \
../src/unidoliMainAPP.c 

OBJS += \
./src/curlGetDat.o \
./src/iot.o \
./src/network.o \
./src/rtmp.o \
./src/types.o \
./src/unidoliMainAPP.o 

C_DEPS += \
./src/curlGetDat.d \
./src/iot.d \
./src/network.d \
./src/rtmp.d \
./src/types.d \
./src/unidoliMainAPP.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-hisiv500-linux-gcc -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libcurl4.4.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/iot-sdk -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/mbedtls -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/modbus -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libunidoli_rtmp -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/libhisi1.0.1.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/wpa_supplicant -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


