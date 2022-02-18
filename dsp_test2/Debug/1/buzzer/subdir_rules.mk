################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
1/buzzer/buzzer.obj: ../1/buzzer/buzzer.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/TI/ccsv6/tools/compiler/ti-cgt-c2000_6.4.2/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/TI/ccsv6/tools/compiler/ti-cgt-c2000_6.4.2/include" --include_path="E:/V101/Example11/BUZZER/1" --include_path="E:/V101/DSP2833x_common/include" --include_path="E:/V101/DSP2833x_headers/include" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="1/buzzer/buzzer.pp" --obj_directory="1/buzzer" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


