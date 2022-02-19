################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main_prothe1.obj" \
"./app/src/adc.obj" \
"./app/src/common.obj" \
"./app/src/i2c.obj" \
"./app/src/imu901.obj" \
"./app/src/leds.obj" \
"./app/src/mpu6050.obj" \
"./app/src/prothesis.obj" \
"./app/src/pwm.obj" \
"./app/src/ringbuffer.obj" \
"./app/src/timer.obj" \
"./app/src/uart.obj" \
"./app/src/eMPL/inv_mpu.obj" \
"./app/src/eMPL/inv_mpu_dmp_motion_driver.obj" \
"./source/DSP2833x_ADC_cal.obj" \
"./source/DSP2833x_Adc.obj" \
"./source/DSP2833x_CodeStartBranch.obj" \
"./source/DSP2833x_CpuTimers.obj" \
"./source/DSP2833x_DefaultIsr.obj" \
"./source/DSP2833x_EPwm.obj" \
"./source/DSP2833x_GlobalVariableDefs.obj" \
"./source/DSP2833x_Gpio.obj" \
"./source/DSP2833x_I2C.obj" \
"./source/DSP2833x_MemCopy.obj" \
"./source/DSP2833x_PieCtrl.obj" \
"./source/DSP2833x_PieVect.obj" \
"./source/DSP2833x_Sci.obj" \
"./source/DSP2833x_Spi.obj" \
"./source/DSP2833x_SysCtrl.obj" \
"./source/DSP2833x_usDelay.obj" \
"../source/DSP2833x_Headers_nonBIOS.cmd" \
"../source/F28335.cmd" \
"../source/IQmath.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include app/src/subdir_vars.mk
-include app/src/eMPL/subdir_vars.mk
-include source/subdir_vars.mk
-include subdir_rules.mk
-include app/src/subdir_rules.mk
-include app/src/eMPL/subdir_rules.mk
-include source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
prothesis_0219.out \

EXE_OUTPUTS__QUOTED += \
"prothesis_0219.out" \

BIN_OUTPUTS += \
prothesis_0219.hex \

BIN_OUTPUTS__QUOTED += \
"prothesis_0219.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "prothesis_0219.out"

# Tool invocations
prothesis_0219.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"prothesis_0219.map" --stack_size=0x300 --warn_sections -i"E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/lib" -i"E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="prothesis_0219_linkInfo.xml" --rom_model -o "prothesis_0219.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

prothesis_0219.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: C2000 Hex Utility'
	"E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/hex2000" --diag_wrap=off -o "prothesis_0219.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main_prothe1.obj" "app\src\adc.obj" "app\src\common.obj" "app\src\i2c.obj" "app\src\imu901.obj" "app\src\leds.obj" "app\src\mpu6050.obj" "app\src\prothesis.obj" "app\src\pwm.obj" "app\src\ringbuffer.obj" "app\src\timer.obj" "app\src\uart.obj" "app\src\eMPL\inv_mpu.obj" "app\src\eMPL\inv_mpu_dmp_motion_driver.obj" "source\DSP2833x_ADC_cal.obj" "source\DSP2833x_Adc.obj" "source\DSP2833x_CodeStartBranch.obj" "source\DSP2833x_CpuTimers.obj" "source\DSP2833x_DefaultIsr.obj" "source\DSP2833x_EPwm.obj" "source\DSP2833x_GlobalVariableDefs.obj" "source\DSP2833x_Gpio.obj" "source\DSP2833x_I2C.obj" "source\DSP2833x_MemCopy.obj" "source\DSP2833x_PieCtrl.obj" "source\DSP2833x_PieVect.obj" "source\DSP2833x_Sci.obj" "source\DSP2833x_Spi.obj" "source\DSP2833x_SysCtrl.obj" "source\DSP2833x_usDelay.obj" 
	-$(RM) "main_prothe1.d" "app\src\adc.d" "app\src\common.d" "app\src\i2c.d" "app\src\imu901.d" "app\src\leds.d" "app\src\mpu6050.d" "app\src\prothesis.d" "app\src\pwm.d" "app\src\ringbuffer.d" "app\src\timer.d" "app\src\uart.d" "app\src\eMPL\inv_mpu.d" "app\src\eMPL\inv_mpu_dmp_motion_driver.d" "source\DSP2833x_Adc.d" "source\DSP2833x_CpuTimers.d" "source\DSP2833x_DefaultIsr.d" "source\DSP2833x_EPwm.d" "source\DSP2833x_GlobalVariableDefs.d" "source\DSP2833x_Gpio.d" "source\DSP2833x_I2C.d" "source\DSP2833x_MemCopy.d" "source\DSP2833x_PieCtrl.d" "source\DSP2833x_PieVect.d" "source\DSP2833x_Sci.d" "source\DSP2833x_Spi.d" "source\DSP2833x_SysCtrl.d" 
	-$(RM) "source\DSP2833x_ADC_cal.d" "source\DSP2833x_CodeStartBranch.d" "source\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

