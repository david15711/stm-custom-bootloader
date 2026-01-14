################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/boot.c \
../Core/Src/bsp.c \
../Core/Src/button.c \
../Core/Src/cli.c \
../Core/Src/dma.c \
../Core/Src/iwdg.c \
../Core/Src/main.c \
../Core/Src/qbuffer.c \
../Core/Src/reset.c \
../Core/Src/rtc.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/uart.c \
../Core/Src/ymodem.c 

OBJS += \
./Core/Src/boot.o \
./Core/Src/bsp.o \
./Core/Src/button.o \
./Core/Src/cli.o \
./Core/Src/dma.o \
./Core/Src/iwdg.o \
./Core/Src/main.o \
./Core/Src/qbuffer.o \
./Core/Src/reset.o \
./Core/Src/rtc.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/uart.o \
./Core/Src/ymodem.o 

C_DEPS += \
./Core/Src/boot.d \
./Core/Src/bsp.d \
./Core/Src/button.d \
./Core/Src/cli.d \
./Core/Src/dma.d \
./Core/Src/iwdg.d \
./Core/Src/main.d \
./Core/Src/qbuffer.d \
./Core/Src/reset.d \
./Core/Src/rtc.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/uart.d \
./Core/Src/ymodem.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32F103xB -DUSER_VECT_TAB_ADDRESS -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/boot.cyclo ./Core/Src/boot.d ./Core/Src/boot.o ./Core/Src/boot.su ./Core/Src/bsp.cyclo ./Core/Src/bsp.d ./Core/Src/bsp.o ./Core/Src/bsp.su ./Core/Src/button.cyclo ./Core/Src/button.d ./Core/Src/button.o ./Core/Src/button.su ./Core/Src/cli.cyclo ./Core/Src/cli.d ./Core/Src/cli.o ./Core/Src/cli.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/iwdg.cyclo ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/iwdg.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/qbuffer.cyclo ./Core/Src/qbuffer.d ./Core/Src/qbuffer.o ./Core/Src/qbuffer.su ./Core/Src/reset.cyclo ./Core/Src/reset.d ./Core/Src/reset.o ./Core/Src/reset.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/uart.cyclo ./Core/Src/uart.d ./Core/Src/uart.o ./Core/Src/uart.su ./Core/Src/ymodem.cyclo ./Core/Src/ymodem.d ./Core/Src/ymodem.o ./Core/Src/ymodem.su

.PHONY: clean-Core-2f-Src

