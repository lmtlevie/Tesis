################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/atomics/lcd/lcd.cpp \
../src/atomics/lcd/main.cpp \
../src/atomics/lcd/moc_lcd.cpp 

OBJS += \
./src/atomics/lcd/lcd.o \
./src/atomics/lcd/main.o \
./src/atomics/lcd/moc_lcd.o 

CPP_DEPS += \
./src/atomics/lcd/lcd.d \
./src/atomics/lcd/main.d \
./src/atomics/lcd/moc_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
src/atomics/lcd/%.o: ../src/atomics/lcd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


