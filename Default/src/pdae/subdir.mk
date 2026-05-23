################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pdae/configure_window.cpp \
../src/pdae/highlighter.cpp \
../src/pdae/main.cpp \
../src/pdae/moc_configure_window.cpp \
../src/pdae/moc_highlighter.cpp \
../src/pdae/moc_pdae.cpp \
../src/pdae/pdae.cpp 

OBJS += \
./src/pdae/configure_window.o \
./src/pdae/highlighter.o \
./src/pdae/main.o \
./src/pdae/moc_configure_window.o \
./src/pdae/moc_highlighter.o \
./src/pdae/moc_pdae.o \
./src/pdae/pdae.o 

CPP_DEPS += \
./src/pdae/configure_window.d \
./src/pdae/highlighter.d \
./src/pdae/main.d \
./src/pdae/moc_configure_window.d \
./src/pdae/moc_highlighter.d \
./src/pdae/moc_pdae.d \
./src/pdae/pdae.d 


# Each subdirectory must supply rules for building sources it contributes
src/pdae/%.o: ../src/pdae/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


