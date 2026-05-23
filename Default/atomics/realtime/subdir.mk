################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/realtime/clock_wait.cpp \
../atomics/realtime/realtimeclock.cpp \
../atomics/realtime/rtdetect.cpp \
../atomics/realtime/rtview_knob.cpp \
../atomics/realtime/rtview_lcd.cpp 

OBJS += \
./atomics/realtime/clock_wait.o \
./atomics/realtime/realtimeclock.o \
./atomics/realtime/rtdetect.o \
./atomics/realtime/rtview_knob.o \
./atomics/realtime/rtview_lcd.o 

CPP_DEPS += \
./atomics/realtime/clock_wait.d \
./atomics/realtime/realtimeclock.d \
./atomics/realtime/rtdetect.d \
./atomics/realtime/rtview_knob.d \
./atomics/realtime/rtview_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/realtime/%.o: ../atomics/realtime/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


