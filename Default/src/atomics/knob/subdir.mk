################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/atomics/knob/main.cpp \
../src/atomics/knob/moc_slider.cpp \
../src/atomics/knob/slider.cpp 

OBJS += \
./src/atomics/knob/main.o \
./src/atomics/knob/moc_slider.o \
./src/atomics/knob/slider.o 

CPP_DEPS += \
./src/atomics/knob/main.d \
./src/atomics/knob/moc_slider.d \
./src/atomics/knob/slider.d 


# Each subdirectory must supply rules for building sources it contributes
src/atomics/knob/%.o: ../src/atomics/knob/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


