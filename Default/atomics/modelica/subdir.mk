################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/modelica/modelica_crossdet.cpp \
../atomics/modelica/modelica_qss_cross_detect.cpp \
../atomics/modelica/modelica_qss_integrator.cpp \
../atomics/modelica/modelica_qss_static.cpp \
../atomics/modelica/modelica_sample.cpp \
../atomics/modelica/modelica_when_discrete.cpp \
../atomics/modelica/outvars.cpp 

OBJS += \
./atomics/modelica/modelica_crossdet.o \
./atomics/modelica/modelica_qss_cross_detect.o \
./atomics/modelica/modelica_qss_integrator.o \
./atomics/modelica/modelica_qss_static.o \
./atomics/modelica/modelica_sample.o \
./atomics/modelica/modelica_when_discrete.o \
./atomics/modelica/outvars.o 

CPP_DEPS += \
./atomics/modelica/modelica_crossdet.d \
./atomics/modelica/modelica_qss_cross_detect.d \
./atomics/modelica/modelica_qss_integrator.d \
./atomics/modelica/modelica_qss_static.d \
./atomics/modelica/modelica_sample.d \
./atomics/modelica/modelica_when_discrete.d \
./atomics/modelica/outvars.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/modelica/%.o: ../atomics/modelica/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


