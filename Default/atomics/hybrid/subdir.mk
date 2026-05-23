################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/hybrid/SimulationsTracker.cpp \
../atomics/hybrid/commsampler.cpp \
../atomics/hybrid/cross3switch.cpp \
../atomics/hybrid/crossdet.cpp \
../atomics/hybrid/crossdetect.cpp \
../atomics/hybrid/crossdetect_sci.cpp \
../atomics/hybrid/crossswitch.cpp \
../atomics/hybrid/diode.cpp \
../atomics/hybrid/dummy.cpp \
../atomics/hybrid/hcomparator.cpp \
../atomics/hybrid/hcomparator_sci.cpp \
../atomics/hybrid/hhysteresis.cpp \
../atomics/hybrid/hquantizer.cpp \
../atomics/hybrid/hquantizer_sci.cpp \
../atomics/hybrid/hsaturation.cpp \
../atomics/hybrid/hsaturation_sci.cpp \
../atomics/hybrid/hswitchtraj.cpp \
../atomics/hybrid/hystcomp.cpp \
../atomics/hybrid/hysteresis_sci.cpp \
../atomics/hybrid/levelcheck.cpp \
../atomics/hybrid/packetchannel.cpp \
../atomics/hybrid/packetdemux.cpp \
../atomics/hybrid/packetgen.cpp \
../atomics/hybrid/packetgen_v2.cpp \
../atomics/hybrid/packethroughput.cpp \
../atomics/hybrid/packetlatency.cpp \
../atomics/hybrid/packetmux.cpp \
../atomics/hybrid/packetqueue.cpp \
../atomics/hybrid/packetred.cpp \
../atomics/hybrid/packetserver.cpp \
../atomics/hybrid/packetsink.cpp \
../atomics/hybrid/packettcprcv.cpp \
../atomics/hybrid/packettcpsnd.cpp \
../atomics/hybrid/qss2quant.cpp \
../atomics/hybrid/rama_inv.cpp \
../atomics/hybrid/runscilabjob.cpp \
../atomics/hybrid/samphold.cpp \
../atomics/hybrid/samphold_sci.cpp \
../atomics/hybrid/stdevstool.cpp \
../atomics/hybrid/switch3.cpp \
../atomics/hybrid/switch3_sci.cpp \
../atomics/hybrid/switchGE.cpp \
../atomics/hybrid/switching.cpp \
../atomics/hybrid/switching2.cpp \
../atomics/hybrid/tokenbucket.cpp \
../atomics/hybrid/trajcross.cpp 

OBJS += \
./atomics/hybrid/SimulationsTracker.o \
./atomics/hybrid/commsampler.o \
./atomics/hybrid/cross3switch.o \
./atomics/hybrid/crossdet.o \
./atomics/hybrid/crossdetect.o \
./atomics/hybrid/crossdetect_sci.o \
./atomics/hybrid/crossswitch.o \
./atomics/hybrid/diode.o \
./atomics/hybrid/dummy.o \
./atomics/hybrid/hcomparator.o \
./atomics/hybrid/hcomparator_sci.o \
./atomics/hybrid/hhysteresis.o \
./atomics/hybrid/hquantizer.o \
./atomics/hybrid/hquantizer_sci.o \
./atomics/hybrid/hsaturation.o \
./atomics/hybrid/hsaturation_sci.o \
./atomics/hybrid/hswitchtraj.o \
./atomics/hybrid/hystcomp.o \
./atomics/hybrid/hysteresis_sci.o \
./atomics/hybrid/levelcheck.o \
./atomics/hybrid/packetchannel.o \
./atomics/hybrid/packetdemux.o \
./atomics/hybrid/packetgen.o \
./atomics/hybrid/packetgen_v2.o \
./atomics/hybrid/packethroughput.o \
./atomics/hybrid/packetlatency.o \
./atomics/hybrid/packetmux.o \
./atomics/hybrid/packetqueue.o \
./atomics/hybrid/packetred.o \
./atomics/hybrid/packetserver.o \
./atomics/hybrid/packetsink.o \
./atomics/hybrid/packettcprcv.o \
./atomics/hybrid/packettcpsnd.o \
./atomics/hybrid/qss2quant.o \
./atomics/hybrid/rama_inv.o \
./atomics/hybrid/runscilabjob.o \
./atomics/hybrid/samphold.o \
./atomics/hybrid/samphold_sci.o \
./atomics/hybrid/stdevstool.o \
./atomics/hybrid/switch3.o \
./atomics/hybrid/switch3_sci.o \
./atomics/hybrid/switchGE.o \
./atomics/hybrid/switching.o \
./atomics/hybrid/switching2.o \
./atomics/hybrid/tokenbucket.o \
./atomics/hybrid/trajcross.o 

CPP_DEPS += \
./atomics/hybrid/SimulationsTracker.d \
./atomics/hybrid/commsampler.d \
./atomics/hybrid/cross3switch.d \
./atomics/hybrid/crossdet.d \
./atomics/hybrid/crossdetect.d \
./atomics/hybrid/crossdetect_sci.d \
./atomics/hybrid/crossswitch.d \
./atomics/hybrid/diode.d \
./atomics/hybrid/dummy.d \
./atomics/hybrid/hcomparator.d \
./atomics/hybrid/hcomparator_sci.d \
./atomics/hybrid/hhysteresis.d \
./atomics/hybrid/hquantizer.d \
./atomics/hybrid/hquantizer_sci.d \
./atomics/hybrid/hsaturation.d \
./atomics/hybrid/hsaturation_sci.d \
./atomics/hybrid/hswitchtraj.d \
./atomics/hybrid/hystcomp.d \
./atomics/hybrid/hysteresis_sci.d \
./atomics/hybrid/levelcheck.d \
./atomics/hybrid/packetchannel.d \
./atomics/hybrid/packetdemux.d \
./atomics/hybrid/packetgen.d \
./atomics/hybrid/packetgen_v2.d \
./atomics/hybrid/packethroughput.d \
./atomics/hybrid/packetlatency.d \
./atomics/hybrid/packetmux.d \
./atomics/hybrid/packetqueue.d \
./atomics/hybrid/packetred.d \
./atomics/hybrid/packetserver.d \
./atomics/hybrid/packetsink.d \
./atomics/hybrid/packettcprcv.d \
./atomics/hybrid/packettcpsnd.d \
./atomics/hybrid/qss2quant.d \
./atomics/hybrid/rama_inv.d \
./atomics/hybrid/runscilabjob.d \
./atomics/hybrid/samphold.d \
./atomics/hybrid/samphold_sci.d \
./atomics/hybrid/stdevstool.d \
./atomics/hybrid/switch3.d \
./atomics/hybrid/switch3_sci.d \
./atomics/hybrid/switchGE.d \
./atomics/hybrid/switching.d \
./atomics/hybrid/switching2.d \
./atomics/hybrid/tokenbucket.d \
./atomics/hybrid/trajcross.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/hybrid/%.o: ../atomics/hybrid/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


