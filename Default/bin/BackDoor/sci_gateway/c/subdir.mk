################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bin/BackDoor/sci_gateway/c/libbackdoor_c.c \
../bin/BackDoor/sci_gateway/c/sci_backdoor.c 

OBJS += \
./bin/BackDoor/sci_gateway/c/libbackdoor_c.o \
./bin/BackDoor/sci_gateway/c/sci_backdoor.o 

C_DEPS += \
./bin/BackDoor/sci_gateway/c/libbackdoor_c.d \
./bin/BackDoor/sci_gateway/c/sci_backdoor.d 


# Each subdirectory must supply rules for building sources it contributes
bin/BackDoor/sci_gateway/c/%.o: ../bin/BackDoor/sci_gateway/c/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


