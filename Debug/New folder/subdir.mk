################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../New\ folder/test_assign1_1.c 

OBJS += \
./New\ folder/test_assign1_1.o 

C_DEPS += \
./New\ folder/test_assign1_1.d 


# Each subdirectory must supply rules for building sources it contributes
New\ folder/test_assign1_1.o: ../New\ folder/test_assign1_1.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -I"C:\cygwin64\lib\gcc\x86_64-pc-cygwin\4.9.3\include" -I"C:\cygwin64\lib\gcc\x86_64-pc-cygwin\4.9.3\include-fixed" -I"C:\cygwin64\usr\include" -I"C:\cygwin64\usr\include\w32api" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"New folder/test_assign1_1.d" -MT"New\ folder/test_assign1_1.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


