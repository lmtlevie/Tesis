################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/common/test_parser/main.cpp \
../src/common/test_parser/parser.cpp \
../src/common/test_parser/scanner.cpp 

OBJS += \
./src/common/test_parser/main.o \
./src/common/test_parser/parser.o \
./src/common/test_parser/scanner.o 

CPP_DEPS += \
./src/common/test_parser/main.d \
./src/common/test_parser/parser.d \
./src/common/test_parser/scanner.d 


# Each subdirectory must supply rules for building sources it contributes
src/common/test_parser/%.o: ../src/common/test_parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


