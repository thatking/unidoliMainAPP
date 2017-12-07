################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/wpa2/src/subdir.mk
-include src/modbus/subdir.mk
-include src/json/subdir.mk
-include src/iot/Debug/src/subdir.mk
-include src/getaddr/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: unidoliMainAPP

# Tool invocations
unidoliMainAPP: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	arm-hisiv500-linux-gcc -L/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libcurl4.4.0/lib -L/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/libunidoli_rtmp -L/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/iot/lib -L/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/libhisi1.0.1.0/lib -L/home/hp/Public/eclipse_workspace/unidoliMainAPP/src/wpa2/wpa_supplicant -o "unidoliMainAPP" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) unidoliMainAPP
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
