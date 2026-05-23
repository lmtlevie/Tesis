################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../engine/connection.cpp \
../engine/coupling.cpp \
../engine/event.cpp \
../engine/model.cpp \
../engine/pdevslib.common.cpp \
../engine/pdevslib.cpp \
../engine/pdevslib.linux.cpp \
../engine/pdevslib.rtai.cpp \
../engine/pdevslib.win.cpp \
../engine/root_coupling.cpp \
../engine/root_simulator.cpp \
../engine/simulator.cpp 

OBJS += \
./engine/connection.o \
./engine/coupling.o \
./engine/event.o \
./engine/model.o \
./engine/pdevslib.common.o \
./engine/pdevslib.o \
./engine/pdevslib.linux.o \
./engine/pdevslib.rtai.o \
./engine/pdevslib.win.o \
./engine/root_coupling.o \
./engine/root_simulator.o \
./engine/simulator.o 

CPP_DEPS += \
./engine/connection.d \
./engine/coupling.d \
./engine/event.d \
./engine/model.d \
./engine/pdevslib.common.d \
./engine/pdevslib.d \
./engine/pdevslib.linux.d \
./engine/pdevslib.rtai.d \
./engine/pdevslib.win.d \
./engine/root_coupling.d \
./engine/root_simulator.d \
./engine/simulator.d 


# Each subdirectory must supply rules for building sources it contributes
engine/%.o: ../engine/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


