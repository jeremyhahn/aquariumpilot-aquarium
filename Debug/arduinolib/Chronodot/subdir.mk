################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../arduinolib/Chronodot/Chronodot.cpp 

OBJS += \
./arduinolib/Chronodot/Chronodot.o 

CPP_DEPS += \
./arduinolib/Chronodot/Chronodot.d 


# Each subdirectory must supply rules for building sources it contributes
arduinolib/Chronodot/%.o: ../arduinolib/Chronodot/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/storage/apps/eclipse-avr/workspace/ArduinoCore-1.0.1/src" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/src" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Ethernet/utility" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Ethernet" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/EEPROM" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Wire/utility" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Wire" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/SPI" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/lib" -DARDUINO=100 -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega2560 -DF_CPU=1600000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


