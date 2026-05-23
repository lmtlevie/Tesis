################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/adquisicion/adquider.cpp \
../atomics/adquisicion/adquidervec.cpp \
../atomics/adquisicion/adquisidor.cpp \
../atomics/adquisicion/adquiutil.cpp \
../atomics/adquisicion/derang.cpp 

OBJS += \
./atomics/adquisicion/adquider.o \
./atomics/adquisicion/adquidervec.o \
./atomics/adquisicion/adquisidor.o \
./atomics/adquisicion/adquiutil.o \
./atomics/adquisicion/derang.o 

CPP_DEPS += \
./atomics/adquisicion/adquider.d \
./atomics/adquisicion/adquidervec.d \
./atomics/adquisicion/adquisidor.d \
./atomics/adquisicion/adquiutil.d \
./atomics/adquisicion/derang.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/adquisicion/%.o: ../atomics/adquisicion/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


