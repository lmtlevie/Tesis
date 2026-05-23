################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bin/BackDoor/src/c/backdoor.c 

OBJS += \
./bin/BackDoor/src/c/backdoor.o 

C_DEPS += \
./bin/BackDoor/src/c/backdoor.d 


# Each subdirectory must supply rules for building sources it contributes
bin/BackDoor/src/c/%.o: ../bin/BackDoor/src/c/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


