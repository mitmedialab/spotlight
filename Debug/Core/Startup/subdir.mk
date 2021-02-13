################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32wb55rgvx.s 

S_DEPS += \
./Core/Startup/startup_stm32wb55rgvx.d 

OBJS += \
./Core/Startup/startup_stm32wb55rgvx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/startup_stm32wb55rgvx.o: ../Core/Startup/startup_stm32wb55rgvx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DARM_MATH_CM4 -c -I"C:/Users/patri/STM32Cube/Repository/Packs/ARM/CMSIS/5.6.0/CMSIS/DSP/Lib/ARM" -I"C:/Users/patri/STM32Cube/Repository/Packs/ARM/CMSIS/5.6.0/CMSIS/DSP/Lib/GCC" -x assembler-with-cpp -MMD -MP -MF"Core/Startup/startup_stm32wb55rgvx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

