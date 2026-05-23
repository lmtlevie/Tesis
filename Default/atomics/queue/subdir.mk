################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/queue/clients.cpp \
../atomics/queue/queue.cpp \
../atomics/queue/server.cpp 

OBJS += \
./atomics/queue/clients.o \
./atomics/queue/queue.o \
./atomics/queue/server.o 

CPP_DEPS += \
./atomics/queue/clients.d \
./atomics/queue/queue.d \
./atomics/queue/server.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/queue/%.o: ../atomics/queue/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


