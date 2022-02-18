################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ADATA/all_work/my_dsp/my_workspace/mpu_test1/app/inc" --include_path="C:/ADATA/all_work/my_dsp/my_workspace/dsp_test2/app/src/eMPL" --include_path="C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --include_path="C:/ADATA/all_work/my_dsp/V101/DSP2833x_common/include" --include_path="C:/ADATA/all_work/my_dsp/V101/DSP2833x_headers/include" --advice:performance=all -g --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ADATA/all_work/my_dsp/my_workspace/mpu_test1/app/inc" --include_path="C:/ADATA/all_work/my_dsp/my_workspace/dsp_test2/app/src/eMPL" --include_path="C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --include_path="C:/ADATA/all_work/my_dsp/V101/DSP2833x_common/include" --include_path="C:/ADATA/all_work/my_dsp/V101/DSP2833x_headers/include" --advice:performance=all -g --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


