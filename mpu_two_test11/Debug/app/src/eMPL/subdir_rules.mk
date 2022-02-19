################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
app/src/eMPL/%.obj: ../app/src/eMPL/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="F:/data/work/my_dsp/mpu_two_test11" --include_path="E:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --include_path="F:/data/work/my_dsp/workspace_v/DSP2833x_headers/include" --include_path="F:/data/work/my_dsp/workspace_v/DSP2833x_common/include" --include_path="F:/data/work/my_dsp/mpu_two_test11/app/inc" --include_path="F:/data/work/my_dsp/mpu_two_test11/app/src/eMPL" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="app/src/eMPL/$(basename $(<F)).d_raw" --obj_directory="app/src/eMPL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


