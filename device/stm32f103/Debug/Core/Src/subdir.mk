################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/usb_device.c \
../Core/Src/usbd.c \
../Core/Src/usbd_conf.c \
../Core/Src/usbd_core.c \
../Core/Src/usbd_ctlreq.c \
../Core/Src/usbd_desc.c \
../Core/Src/usbd_ioreq.c 

OBJS += \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/usb_device.o \
./Core/Src/usbd.o \
./Core/Src/usbd_conf.o \
./Core/Src/usbd_core.o \
./Core/Src/usbd_ctlreq.o \
./Core/Src/usbd_desc.o \
./Core/Src/usbd_ioreq.o 

C_DEPS += \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/usb_device.d \
./Core/Src/usbd.d \
./Core/Src/usbd_conf.d \
./Core/Src/usbd_core.d \
./Core/Src/usbd_ctlreq.d \
./Core/Src/usbd_desc.d \
./Core/Src/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/usb_device.d ./Core/Src/usb_device.o ./Core/Src/usbd.d ./Core/Src/usbd.o ./Core/Src/usbd_conf.d ./Core/Src/usbd_conf.o ./Core/Src/usbd_core.d ./Core/Src/usbd_core.o ./Core/Src/usbd_ctlreq.d ./Core/Src/usbd_ctlreq.o ./Core/Src/usbd_desc.d ./Core/Src/usbd_desc.o ./Core/Src/usbd_ioreq.d ./Core/Src/usbd_ioreq.o

.PHONY: clean-Core-2f-Src

