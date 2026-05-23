################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/signal/filter.cpp \
../atomics/signal/modulator.cpp \
../atomics/signal/scalar2vec.cpp \
../atomics/signal/vec2scalar.cpp 

OBJS += \
./atomics/signal/filter.o \
./atomics/signal/modulator.o \
./atomics/signal/scalar2vec.o \
./atomics/signal/vec2scalar.o 

CPP_DEPS += \
./atomics/signal/filter.d \
./atomics/signal/modulator.d \
./atomics/signal/scalar2vec.d \
./atomics/signal/vec2scalar.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/signal/%.o: ../atomics/signal/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


