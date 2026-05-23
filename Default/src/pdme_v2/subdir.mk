################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pdme_v2/commands.cpp \
../src/pdme_v2/globals.cpp \
../src/pdme_v2/main.cpp \
../src/pdme_v2/powergui.cpp \
../src/pdme_v2/qrc_pdme.cpp 

OBJS += \
./src/pdme_v2/commands.o \
./src/pdme_v2/globals.o \
./src/pdme_v2/main.o \
./src/pdme_v2/powergui.o \
./src/pdme_v2/qrc_pdme.o 

CPP_DEPS += \
./src/pdme_v2/commands.d \
./src/pdme_v2/globals.d \
./src/pdme_v2/main.d \
./src/pdme_v2/powergui.d \
./src/pdme_v2/qrc_pdme.d 


# Each subdirectory must supply rules for building sources it contributes
src/pdme_v2/%.o: ../src/pdme_v2/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


