################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/atomics/rtview/qtsingleapplication/examples/trivial/main.cpp 

OBJS += \
./src/atomics/rtview/qtsingleapplication/examples/trivial/main.o 

CPP_DEPS += \
./src/atomics/rtview/qtsingleapplication/examples/trivial/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/atomics/rtview/qtsingleapplication/examples/trivial/%.o: ../src/atomics/rtview/qtsingleapplication/examples/trivial/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


