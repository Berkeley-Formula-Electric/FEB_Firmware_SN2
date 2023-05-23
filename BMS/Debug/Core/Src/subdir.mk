################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/FEB_BMS_Precharge.c \
../Core/Src/FEB_BMS_Shutdown.c \
../Core/Src/FEB_BMS_State.c \
../Core/Src/FEB_CAN.c \
../Core/Src/FEB_CAN_Charger.c \
../Core/Src/FEB_CAN_IVT.c \
../Core/Src/FEB_Fan.c \
../Core/Src/FEB_LTC6811.c \
../Core/Src/FEB_LTC6811_Temp_LUT.c \
../Core/Src/LTC6811.c \
../Core/Src/LTC681x.c \
../Core/Src/bms_hardware.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/FEB_BMS_Precharge.o \
./Core/Src/FEB_BMS_Shutdown.o \
./Core/Src/FEB_BMS_State.o \
./Core/Src/FEB_CAN.o \
./Core/Src/FEB_CAN_Charger.o \
./Core/Src/FEB_CAN_IVT.o \
./Core/Src/FEB_Fan.o \
./Core/Src/FEB_LTC6811.o \
./Core/Src/FEB_LTC6811_Temp_LUT.o \
./Core/Src/LTC6811.o \
./Core/Src/LTC681x.o \
./Core/Src/bms_hardware.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/FEB_BMS_Precharge.d \
./Core/Src/FEB_BMS_Shutdown.d \
./Core/Src/FEB_BMS_State.d \
./Core/Src/FEB_CAN.d \
./Core/Src/FEB_CAN_Charger.d \
./Core/Src/FEB_CAN_IVT.d \
./Core/Src/FEB_Fan.d \
./Core/Src/FEB_LTC6811.d \
./Core/Src/FEB_LTC6811_Temp_LUT.d \
./Core/Src/LTC6811.d \
./Core/Src/LTC681x.d \
./Core/Src/bms_hardware.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/FEB_BMS_Precharge.d ./Core/Src/FEB_BMS_Precharge.o ./Core/Src/FEB_BMS_Precharge.su ./Core/Src/FEB_BMS_Shutdown.d ./Core/Src/FEB_BMS_Shutdown.o ./Core/Src/FEB_BMS_Shutdown.su ./Core/Src/FEB_BMS_State.d ./Core/Src/FEB_BMS_State.o ./Core/Src/FEB_BMS_State.su ./Core/Src/FEB_CAN.d ./Core/Src/FEB_CAN.o ./Core/Src/FEB_CAN.su ./Core/Src/FEB_CAN_Charger.d ./Core/Src/FEB_CAN_Charger.o ./Core/Src/FEB_CAN_Charger.su ./Core/Src/FEB_CAN_IVT.d ./Core/Src/FEB_CAN_IVT.o ./Core/Src/FEB_CAN_IVT.su ./Core/Src/FEB_Fan.d ./Core/Src/FEB_Fan.o ./Core/Src/FEB_Fan.su ./Core/Src/FEB_LTC6811.d ./Core/Src/FEB_LTC6811.o ./Core/Src/FEB_LTC6811.su ./Core/Src/FEB_LTC6811_Temp_LUT.d ./Core/Src/FEB_LTC6811_Temp_LUT.o ./Core/Src/FEB_LTC6811_Temp_LUT.su ./Core/Src/LTC6811.d ./Core/Src/LTC6811.o ./Core/Src/LTC6811.su ./Core/Src/LTC681x.d ./Core/Src/LTC681x.o ./Core/Src/LTC681x.su ./Core/Src/bms_hardware.d ./Core/Src/bms_hardware.o ./Core/Src/bms_hardware.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

