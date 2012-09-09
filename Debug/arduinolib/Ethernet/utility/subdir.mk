################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../arduinolib/Ethernet/utility/socket.cpp \
../arduinolib/Ethernet/utility/w5100.cpp 

OBJS += \
./arduinolib/Ethernet/utility/socket.o \
./arduinolib/Ethernet/utility/w5100.o 

CPP_DEPS += \
./arduinolib/Ethernet/utility/socket.d \
./arduinolib/Ethernet/utility/w5100.d 


# Each subdirectory must supply rules for building sources it contributes
arduinolib/Ethernet/utility/%.o: ../arduinolib/Ethernet/utility/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/storage/apps/eclipse-avr/workspace/ArduinoCore-1.0/src" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/src" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/lib" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Wire/utility" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Wire" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Ethernet/utility" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/Ethernet" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/SPI" -I"/storage/apps/eclipse-avr/workspace/aquariumpilot-aquarium/arduinolib/EEPROM" -DARDUINO=100 -Wall -g2 -gstabs -O0 -ffunction-sections -fdata-sections -fno-exceptions -mmcu=atmega2560 -DF_CPU=1600000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


