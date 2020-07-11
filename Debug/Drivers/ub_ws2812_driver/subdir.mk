################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ub_ws2812_driver/stm32_ub_ws2812.c 

OBJS += \
./Drivers/ub_ws2812_driver/stm32_ub_ws2812.o 

C_DEPS += \
./Drivers/ub_ws2812_driver/stm32_ub_ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ub_ws2812_driver/stm32_ub_ws2812.o: ../Drivers/ub_ws2812_driver/stm32_ub_ws2812.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/ub_ws2812_driver" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/ub_ws2812_driver/stm32_ub_ws2812.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

