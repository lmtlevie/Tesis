################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/atomics/rtview/knob.cpp \
../src/atomics/rtview/lcd.cpp \
../src/atomics/rtview/main.cpp \
../src/atomics/rtview/moc_knob.cpp \
../src/atomics/rtview/moc_lcd.cpp \
../src/atomics/rtview/moc_qtlocalpeer.cpp \
../src/atomics/rtview/moc_qtsingleapplication.cpp \
../src/atomics/rtview/moc_rtview.cpp \
../src/atomics/rtview/rtview.cpp 

OBJS += \
./src/atomics/rtview/knob.o \
./src/atomics/rtview/lcd.o \
./src/atomics/rtview/main.o \
./src/atomics/rtview/moc_knob.o \
./src/atomics/rtview/moc_lcd.o \
./src/atomics/rtview/moc_qtlocalpeer.o \
./src/atomics/rtview/moc_qtsingleapplication.o \
./src/atomics/rtview/moc_rtview.o \
./src/atomics/rtview/rtview.o 

CPP_DEPS += \
./src/atomics/rtview/knob.d \
./src/atomics/rtview/lcd.d \
./src/atomics/rtview/main.d \
./src/atomics/rtview/moc_knob.d \
./src/atomics/rtview/moc_lcd.d \
./src/atomics/rtview/moc_qtlocalpeer.d \
./src/atomics/rtview/moc_qtsingleapplication.d \
./src/atomics/rtview/moc_rtview.d \
./src/atomics/rtview/rtview.d 


# Each subdirectory must supply rules for building sources it contributes
src/atomics/rtview/%.o: ../src/atomics/rtview/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


