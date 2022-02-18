################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
2/main.obj: ../2/main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/TI/ccsv6/tools/compiler/ti-cgt-c2000_6.4.2/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/V101/Example11/Step_Motor/1/key" --include_path="E:/V101/Example11/Step_Motor/1/LEDS" --include_path="E:/V101/Example11/Step_Motor/1/step_motor" --include_path="E:/V101/Example11/DC_Motor/1/dc_motor" --include_path="E:/V101/Example11/Key/1/buzzer" --include_path="E:/V101/Example11/Key/1/relay" --include_path="C:/TI/ccsv6/tools/compiler/ti-cgt-c2000_6.4.2/include" --include_path="E:/V101/DSP2833x_common/include" --include_path="E:/V101/DSP2833x_headers/include" --include_path="/packages/ti/xdais" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="2/main.pp" --obj_directory="2" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


