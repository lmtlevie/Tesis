################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pdif/main.cpp \
../src/pdif/moc_pdif.cpp \
../src/pdif/pdif.cpp \
../src/pdif/qrc_pdif.cpp 

OBJS += \
./src/pdif/main.o \
./src/pdif/moc_pdif.o \
./src/pdif/pdif.o \
./src/pdif/qrc_pdif.o 

CPP_DEPS += \
./src/pdif/main.d \
./src/pdif/moc_pdif.d \
./src/pdif/pdif.d \
./src/pdif/qrc_pdif.d 


# Each subdirectory must supply rules for building sources it contributes
src/pdif/%.o: ../src/pdif/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


