################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
app/src/eMPL/%.obj: ../app/src/eMPL/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/Program Files (x86)/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/Program Files (x86)/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/ADATA/all_work/my_dsp/my_workspace/dsp_test2/app/inc" --include_path="C:/ADATA/all_work/my_dsp/V101/DSP2833x_common/include" --include_path="C:/ADATA/all_work/my_dsp/V101/DSP2833x_headers/include" --advice:performance=all -g --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="app/src/eMPL/$(basename $(<F)).d_raw" --obj_directory="app/src/eMPL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


