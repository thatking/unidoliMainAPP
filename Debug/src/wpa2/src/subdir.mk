################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/wpa2/src/audio_transfer_fft.c \
../src/wpa2/src/sample_comm_sys.c \
../src/wpa2/src/unidoli_audio.c \
../src/wpa2/src/wpa2APP.c 

OBJS += \
./src/wpa2/src/audio_transfer_fft.o \
./src/wpa2/src/sample_comm_sys.o \
./src/wpa2/src/unidoli_audio.o \
./src/wpa2/src/wpa2APP.o 

C_DEPS += \
./src/wpa2/src/audio_transfer_fft.d \
./src/wpa2/src/sample_comm_sys.d \
./src/wpa2/src/unidoli_audio.d \
./src/wpa2/src/wpa2APP.d 


# Each subdirectory must supply rules for building sources it contributes
src/wpa2/src/%.o: ../src/wpa2/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-hisiv500-linux-gcc -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libcurl4.4.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/iot-sdk -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/include/mbedtls -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/modbus -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libunidoli_rtmp -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/libhisi1.0.1.0/include -I/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/wpa_supplicant -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


