################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/getaddr/getMacAddr.c 

OBJS += \
./src/getaddr/getMacAddr.o 

C_DEPS += \
./src/getaddr/getMacAddr.d 


# Each subdirectory must supply rules for building sources it contributes
src/getaddr/%.o: ../src/getaddr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-hisiv500-linux-gcc -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libcurl4.4.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/iot-sdk -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/mbedtls -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/modbus -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libunidoli_rtmp -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/libhisi1.0.1.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/wpa_supplicant -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


