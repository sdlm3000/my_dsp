################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SRC/%.obj: ../SRC/%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/Program Files (x86)/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --include_path="C:/Program Files (x86)/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/ADATA/all_work/my_dsp/my_workspace/I2C_MPU6050/INCLUDE" --include_path="C:/ADATA/all_work/my_dsp/workspace_v/DSP2833x_common/include" --include_path="C:/ADATA/all_work/my_dsp/workspace_v/DSP2833x_headers/include" --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRC/%.obj: ../SRC/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/Program Files (x86)/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --include_path="C:/Program Files (x86)/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/ADATA/all_work/my_dsp/my_workspace/I2C_MPU6050/INCLUDE" --include_path="C:/ADATA/all_work/my_dsp/workspace_v/DSP2833x_common/include" --include_path="C:/ADATA/all_work/my_dsp/workspace_v/DSP2833x_headers/include" --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


