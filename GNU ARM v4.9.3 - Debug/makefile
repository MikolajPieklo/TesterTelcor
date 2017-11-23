################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emusb/subdir.mk
-include emlib/subdir.mk
-include Drivers/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
-include BSP/subdir.mk
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
all: TesterTelcor.axf

# Tool invocations
TesterTelcor.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "TesterTelcor.ld" -Xlinker --gc-sections -Xlinker -Map="TesterTelcor.map" --specs=nano.specs -o TesterTelcor.axf "./BSP/bsp_bcc.o" "./BSP/bsp_stk.o" "./BSP/bsp_stk_leds.o" "./BSP/bsp_trace.o" "./CMSIS/EFM32GG/startup_gcc_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./Drivers/cdc.o" "./Drivers/dmactrl.o" "./Drivers/eeprom.o" "./Drivers/msdd.o" "./Drivers/segmentlcd.o" "./emlib/em_adc.o" "./emlib/em_assert.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_lcd.o" "./emlib/em_msc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emusb/em_usbd.o" "./emusb/em_usbdch9.o" "./emusb/em_usbdep.o" "./emusb/em_usbdint.o" "./emusb/em_usbhal.o" "./emusb/em_usbtimer.o" "./src/ADC_EFM32.o" "./src/ADS1247.o" "./src/MCP23017.o" "./src/Modbus.o" "./src/RS485.o" "./src/STC3105.o" "./src/descriptors.o" "./src/gpiointerrupt.o" "./src/main.o" "./src/msddmedia.o" "./src/myUSB.o" "./src/my_timer.o" "./src/tester.o" "./src/vud.o" "./src/zadania.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: TesterTelcor.hex'
	arm-none-eabi-objcopy -O ihex "TesterTelcor.axf" "TesterTelcor.hex"
	@echo ' '

	@echo 'Building bin file: TesterTelcor.bin'
	arm-none-eabi-objcopy -O binary "TesterTelcor.axf" "TesterTelcor.bin"
	@echo ' '

	@echo 'Building s37 file: TesterTelcor.s37'
	arm-none-eabi-objcopy -O srec "TesterTelcor.axf" "TesterTelcor.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "TesterTelcor.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TesterTelcor.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
