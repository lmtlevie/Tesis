################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/random/lib/erfres.cpp \
../atomics/random/lib/erfresmk.cpp \
../atomics/random/lib/fnchyppr.cpp \
../atomics/random/lib/mersenne.cpp \
../atomics/random/lib/mother.cpp \
../atomics/random/lib/rancombi.cpp \
../atomics/random/lib/randomc.cpp \
../atomics/random/lib/sfmt.cpp \
../atomics/random/lib/stoc2.cpp \
../atomics/random/lib/stoc3.cpp \
../atomics/random/lib/stocc.cpp \
../atomics/random/lib/testrandomac.cpp \
../atomics/random/lib/userintf.cpp \
../atomics/random/lib/wnchyppr.cpp 

OBJS += \
./atomics/random/lib/erfres.o \
./atomics/random/lib/erfresmk.o \
./atomics/random/lib/fnchyppr.o \
./atomics/random/lib/mersenne.o \
./atomics/random/lib/mother.o \
./atomics/random/lib/rancombi.o \
./atomics/random/lib/randomc.o \
./atomics/random/lib/sfmt.o \
./atomics/random/lib/stoc2.o \
./atomics/random/lib/stoc3.o \
./atomics/random/lib/stocc.o \
./atomics/random/lib/testrandomac.o \
./atomics/random/lib/userintf.o \
./atomics/random/lib/wnchyppr.o 

CPP_DEPS += \
./atomics/random/lib/erfres.d \
./atomics/random/lib/erfresmk.d \
./atomics/random/lib/fnchyppr.d \
./atomics/random/lib/mersenne.d \
./atomics/random/lib/mother.d \
./atomics/random/lib/rancombi.d \
./atomics/random/lib/randomc.d \
./atomics/random/lib/sfmt.d \
./atomics/random/lib/stoc2.d \
./atomics/random/lib/stoc3.d \
./atomics/random/lib/stocc.d \
./atomics/random/lib/testrandomac.d \
./atomics/random/lib/userintf.d \
./atomics/random/lib/wnchyppr.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/random/lib/%.o: ../atomics/random/lib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


