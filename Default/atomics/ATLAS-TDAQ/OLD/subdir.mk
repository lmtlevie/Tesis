################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/ATLAS-TDAQ/OLD/ScalarIp2DcmToRVector.cpp \
../atomics/ATLAS-TDAQ/OLD/ScalarIp2DcmVector.cpp \
../atomics/ATLAS-TDAQ/OLD/VectorDcm2DcmToRVector.cpp \
../atomics/ATLAS-TDAQ/OLD/VectorIpDst2ToRQueueVector.cpp \
../atomics/ATLAS-TDAQ/OLD/VectorTcpSender2RosIndex.cpp \
../atomics/ATLAS-TDAQ/OLD/tokenbucket.cpp 

OBJS += \
./atomics/ATLAS-TDAQ/OLD/ScalarIp2DcmToRVector.o \
./atomics/ATLAS-TDAQ/OLD/ScalarIp2DcmVector.o \
./atomics/ATLAS-TDAQ/OLD/VectorDcm2DcmToRVector.o \
./atomics/ATLAS-TDAQ/OLD/VectorIpDst2ToRQueueVector.o \
./atomics/ATLAS-TDAQ/OLD/VectorTcpSender2RosIndex.o \
./atomics/ATLAS-TDAQ/OLD/tokenbucket.o 

CPP_DEPS += \
./atomics/ATLAS-TDAQ/OLD/ScalarIp2DcmToRVector.d \
./atomics/ATLAS-TDAQ/OLD/ScalarIp2DcmVector.d \
./atomics/ATLAS-TDAQ/OLD/VectorDcm2DcmToRVector.d \
./atomics/ATLAS-TDAQ/OLD/VectorIpDst2ToRQueueVector.d \
./atomics/ATLAS-TDAQ/OLD/VectorTcpSender2RosIndex.d \
./atomics/ATLAS-TDAQ/OLD/tokenbucket.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/ATLAS-TDAQ/OLD/%.o: ../atomics/ATLAS-TDAQ/OLD/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


