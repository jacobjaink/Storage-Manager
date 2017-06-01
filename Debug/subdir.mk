################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DoublyLinkedList.c \
../buffer_mgr.c \
../buffer_mgr_stat.c \
../dberror.c \
../storage_mgr.c \
../test_assign2_1.c 

OBJS += \
./DoublyLinkedList.o \
./buffer_mgr.o \
./buffer_mgr_stat.o \
./dberror.o \
./storage_mgr.o \
./test_assign2_1.o 

C_DEPS += \
./DoublyLinkedList.d \
./buffer_mgr.d \
./buffer_mgr_stat.d \
./dberror.d \
./storage_mgr.d \
./test_assign2_1.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -I"C:\cygwin64\lib\gcc\x86_64-pc-cygwin\4.9.3\include" -I"C:\cygwin64\lib\gcc\x86_64-pc-cygwin\4.9.3\include-fixed" -I"C:\cygwin64\usr\include" -I"C:\cygwin64\usr\include\w32api" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


