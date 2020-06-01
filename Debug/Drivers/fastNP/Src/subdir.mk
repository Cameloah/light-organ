################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/fastNP/Src/demo_main.c \
../Drivers/fastNP/Src/main.c \
../Drivers/fastNP/Src/stm32f1xx_hal_msp.c \
../Drivers/fastNP/Src/stm32f1xx_it.c \
../Drivers/fastNP/Src/system_stm32f1xx.c \
../Drivers/fastNP/Src/ws2812_led.c 

OBJS += \
./Drivers/fastNP/Src/demo_main.o \
./Drivers/fastNP/Src/main.o \
./Drivers/fastNP/Src/stm32f1xx_hal_msp.o \
./Drivers/fastNP/Src/stm32f1xx_it.o \
./Drivers/fastNP/Src/system_stm32f1xx.o \
./Drivers/fastNP/Src/ws2812_led.o 

C_DEPS += \
./Drivers/fastNP/Src/demo_main.d \
./Drivers/fastNP/Src/main.d \
./Drivers/fastNP/Src/stm32f1xx_hal_msp.d \
./Drivers/fastNP/Src/stm32f1xx_it.d \
./Drivers/fastNP/Src/system_stm32f1xx.d \
./Drivers/fastNP/Src/ws2812_led.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/fastNP/Src/demo_main.o: ../Drivers/fastNP/Src/demo_main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/fastNP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/fastNP/Src/demo_main.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/fastNP/Src/main.o: ../Drivers/fastNP/Src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/fastNP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/fastNP/Src/main.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/fastNP/Src/stm32f1xx_hal_msp.o: ../Drivers/fastNP/Src/stm32f1xx_hal_msp.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/fastNP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/fastNP/Src/stm32f1xx_hal_msp.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/fastNP/Src/stm32f1xx_it.o: ../Drivers/fastNP/Src/stm32f1xx_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/fastNP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/fastNP/Src/stm32f1xx_it.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/fastNP/Src/system_stm32f1xx.o: ../Drivers/fastNP/Src/system_stm32f1xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/fastNP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/fastNP/Src/system_stm32f1xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/fastNP/Src/ws2812_led.o: ../Drivers/fastNP/Src/ws2812_led.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xE -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Bibliotheken/Dokumente/Git/light-organ/Drivers/fastNP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/fastNP/Src/ws2812_led.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

