################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/petri/database.cpp \
../atomics/petri/event_triggered_transition.cpp \
../atomics/petri/linkedlist.cpp \
../atomics/petri/place.cpp \
../atomics/petri/transition.cpp 

OBJS += \
./atomics/petri/database.o \
./atomics/petri/event_triggered_transition.o \
./atomics/petri/linkedlist.o \
./atomics/petri/place.o \
./atomics/petri/transition.o 

CPP_DEPS += \
./atomics/petri/database.d \
./atomics/petri/event_triggered_transition.d \
./atomics/petri/linkedlist.d \
./atomics/petri/place.d \
./atomics/petri/transition.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/petri/%.o: ../atomics/petri/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


