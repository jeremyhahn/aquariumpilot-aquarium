################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include lib/subdir.mk
-include arduinolib/Wire/utility/subdir.mk
-include arduinolib/Wire/subdir.mk
-include arduinolib/SPI/subdir.mk
-include arduinolib/Ethernet/utility/subdir.mk
-include arduinolib/Ethernet/subdir.mk
-include arduinolib/EEPROM/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
FLASH_IMAGE += \
aquariumpilot-aquarium.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: aquariumpilot-aquarium.elf secondary-outputs

# Tool invocations
aquariumpilot-aquarium.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,aquariumpilot-aquarium.map,--cref -L"/storage/apps/eclipse-avr/workspace/ArduinoCore-1.0/Mega2560" -Xlinker-Wl,--gc-sections -mmcu=atmega2560 -o "aquariumpilot-aquarium.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

aquariumpilot-aquarium.hex: aquariumpilot-aquarium.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex aquariumpilot-aquarium.elf  "aquariumpilot-aquarium.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: aquariumpilot-aquarium.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega2560 aquariumpilot-aquarium.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ELFS)$(FLASH_IMAGE)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) aquariumpilot-aquarium.elf
	-@echo ' '

secondary-outputs: $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
